#include "JsonStorage.h"

#include <Windows.h>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

namespace {
const fs::path dataRoot = "data";
const fs::path usersRoot = dataRoot / "users";
const fs::path bookingsRoot = dataRoot / "bookings";
const fs::path paymentsRoot = dataRoot / "payments";
const fs::path invoicesRoot = dataRoot / "invoices";

string stableId(const string& prefix, const string& value) {
	uint64_t hash = 14695981039346656037ull;
	for (unsigned char ch : value) {
		hash ^= ch;
		hash *= 1099511628211ull;
	}
	ostringstream output;
	output << prefix << hex << setw(16) << setfill('0') << hash;
	return output.str();
}

string escapeJson(const string& value) {
	ostringstream output;
	for (unsigned char ch : value) {
		switch (ch) {
		case '"': output << "\\\""; break;
		case '\\': output << "\\\\"; break;
		case '\b': output << "\\b"; break;
		case '\f': output << "\\f"; break;
		case '\n': output << "\\n"; break;
		case '\r': output << "\\r"; break;
		case '\t': output << "\\t"; break;
		default:
			if (ch < 0x20) {
				output << "\\u" << hex << setw(4) << setfill('0') << static_cast<int>(ch) << dec;
			}
			else {
				output << static_cast<char>(ch);
			}
		}
	}
	return output.str();
}

optional<string> stringField(const string& json, const string& key) {
	const string token = "\"" + key + "\"";
	size_t position = json.find(token);
	if (position == string::npos) return nullopt;
	position = json.find(':', position + token.size());
	if (position == string::npos) return nullopt;
	position = json.find('"', position + 1);
	if (position == string::npos) return nullopt;

	string value;
	bool escaped = false;
	for (++position; position < json.size(); ++position) {
		char ch = json[position];
		if (escaped) {
			switch (ch) {
			case 'b': value += '\b'; break;
			case 'f': value += '\f'; break;
			case 'n': value += '\n'; break;
			case 'r': value += '\r'; break;
			case 't': value += '\t'; break;
			default: value += ch; break;
			}
			escaped = false;
		}
		else if (ch == '\\') {
			escaped = true;
		}
		else if (ch == '"') {
			return value;
		}
		else {
			value += ch;
		}
	}
	return nullopt;
}

optional<int> intField(const string& json, const string& key) {
	const string token = "\"" + key + "\"";
	size_t position = json.find(token);
	if (position == string::npos) return nullopt;
	position = json.find(':', position + token.size());
	if (position == string::npos) return nullopt;
	try {
		size_t used = 0;
		int value = stoi(json.substr(position + 1), &used);
		return value;
	}
	catch (...) {
		return nullopt;
	}
}

vector<string> objectArray(const string& json, const string& key) {
	vector<string> objects;
	const string token = "\"" + key + "\"";
	size_t position = json.find(token);
	if (position == string::npos) return objects;
	position = json.find('[', position + token.size());
	if (position == string::npos) return objects;

	bool inString = false;
	bool escaped = false;
	int depth = 0;
	size_t objectStart = string::npos;
	for (++position; position < json.size(); ++position) {
		char ch = json[position];
		if (inString) {
			if (escaped) escaped = false;
			else if (ch == '\\') escaped = true;
			else if (ch == '"') inString = false;
			continue;
		}
		if (ch == '"') inString = true;
		else if (ch == '{') {
			if (depth++ == 0) objectStart = position;
		}
		else if (ch == '}' && depth > 0) {
			if (--depth == 0 && objectStart != string::npos) {
				objects.push_back(json.substr(objectStart, position - objectStart + 1));
			}
		}
		else if (ch == ']' && depth == 0) break;
	}
	return objects;
}

string readAll(const fs::path& path) {
	ifstream input(path, ios::binary);
	if (!input) return {};
	return string(istreambuf_iterator<char>(input), istreambuf_iterator<char>());
}

string passengerJson(const PassengerRecord& passenger, const string& indent) {
	ostringstream output;
	output << indent << "{\n"
		<< indent << "  \"firstName\": \"" << escapeJson(passenger.firstName) << "\",\n"
		<< indent << "  \"lastName\": \"" << escapeJson(passenger.lastName) << "\",\n"
		<< indent << "  \"passport\": \"" << escapeJson(passenger.passport) << "\",\n"
		<< indent << "  \"contactFirstName\": \"" << escapeJson(passenger.contactFirstName) << "\",\n"
		<< indent << "  \"contactLastName\": \"" << escapeJson(passenger.contactLastName) << "\",\n"
		<< indent << "  \"contactMobile\": \"" << escapeJson(passenger.contactMobile) << "\"\n"
		<< indent << "}";
	return output.str();
}

PassengerRecord parsePassenger(const string& json) {
	PassengerRecord passenger;
	passenger.firstName = stringField(json, "firstName").value_or("");
	passenger.lastName = stringField(json, "lastName").value_or("");
	passenger.passport = stringField(json, "passport").value_or("");
	passenger.contactFirstName = stringField(json, "contactFirstName").value_or("");
	passenger.contactLastName = stringField(json, "contactLastName").value_or("");
	passenger.contactMobile = stringField(json, "contactMobile").value_or("");
	return passenger;
}

bool parseUser(const string& json, UserRecord& user) {
	auto firstName = stringField(json, "firstName");
	auto lastName = stringField(json, "lastName");
	auto mobile = stringField(json, "mobile");
	auto email = stringField(json, "email");
	auto username = stringField(json, "username");
	auto password = stringField(json, "password");
	if (!firstName || !lastName || !mobile || !email || !username || !password) return false;
	user = { *firstName, *lastName, *mobile, *email, *username, *password };
	return true;
}
}

bool writeTextAtomically(const fs::path& path, const string& content) {
	error_code error;
	fs::create_directories(path.parent_path(), error);
	if (error) return false;

	fs::path temporary = path;
	temporary += ".tmp";
	fs::remove(temporary, error);
	ofstream output(temporary, ios::binary | ios::trunc);
	if (!output) return false;
	output << content;
	output.flush();
	if (!output) return false;
	output.close();
	return MoveFileExW(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

bool initializeJsonStorage() {
	error_code error;
	fs::create_directories(usersRoot, error);
	fs::create_directories(bookingsRoot, error);
	fs::create_directories(paymentsRoot, error);
	fs::create_directories(invoicesRoot, error);
	if (error) return false;

	if (!fs::exists("user.txt") || !loadUsersJson().empty()) return true;
	ifstream legacy("user.txt");
	UserRecord user;
	while (getline(legacy, user.firstName) && getline(legacy, user.lastName) &&
		getline(legacy, user.mobile) && getline(legacy, user.email) &&
		getline(legacy, user.username) && getline(legacy, user.password)) {
		if (!saveUserJson(user)) return false;
	}
	return true;
}

vector<UserRecord> loadUsersJson() {
	vector<UserRecord> users;
	error_code error;
	if (!fs::exists(usersRoot)) return users;
	for (const auto& entry : fs::directory_iterator(usersRoot, error)) {
		if (error) break;
		if (!entry.is_regular_file() || entry.path().extension() != ".json") continue;
		UserRecord user;
		if (parseUser(readAll(entry.path()), user)) users.push_back(user);
	}
	return users;
}

bool saveUserJson(const UserRecord& user) {
	ostringstream json;
	json << "{\n"
		<< "  \"userId\": \"" << stableId("usr_", user.username) << "\",\n"
		<< "  \"firstName\": \"" << escapeJson(user.firstName) << "\",\n"
		<< "  \"lastName\": \"" << escapeJson(user.lastName) << "\",\n"
		<< "  \"mobile\": \"" << escapeJson(user.mobile) << "\",\n"
		<< "  \"email\": \"" << escapeJson(user.email) << "\",\n"
		<< "  \"username\": \"" << escapeJson(user.username) << "\",\n"
		<< "  \"password\": \"" << escapeJson(user.password) << "\"\n"
		<< "}\n";
	return writeTextAtomically(usersRoot / (stableId("usr_", user.username) + ".json"), json.str());
}

string bookingIdForUser(const string& username) {
	return stableId("bkg_", username);
}

bool bookingJsonExists(const string& username) {
	return fs::exists(bookingsRoot / (bookingIdForUser(username) + ".json"));
}

bool loadBookingJson(const string& username, BookingRecord& booking) {
	string json = readAll(bookingsRoot / (bookingIdForUser(username) + ".json"));
	if (json.empty()) return false;
	auto bookingId = stringField(json, "bookingId");
	auto storedUsername = stringField(json, "username");
	auto departureFlight = intField(json, "departureFlight");
	auto departureDate = stringField(json, "departureDate");
	auto departureSlot = intField(json, "departureSlot");
	auto returnFlight = intField(json, "returnFlight");
	auto returnDate = stringField(json, "returnDate");
	auto returnSlot = intField(json, "returnSlot");
	if (!bookingId || !storedUsername || !departureFlight || !departureDate || !departureSlot ||
		!returnFlight || !returnDate || !returnSlot || *storedUsername != username) return false;
	booking = { *bookingId, *storedUsername, *departureFlight, *departureDate, *departureSlot,
		*returnFlight, *returnDate, *returnSlot, {} };
	for (const string& item : objectArray(json, "passengers")) booking.passengers.push_back(parsePassenger(item));
	return !booking.passengers.empty();
}

bool saveBookingJson(const BookingRecord& booking) {
	ostringstream json;
	json << "{\n"
		<< "  \"bookingId\": \"" << escapeJson(booking.bookingId) << "\",\n"
		<< "  \"username\": \"" << escapeJson(booking.username) << "\",\n"
		<< "  \"departureFlight\": " << booking.departureFlight << ",\n"
		<< "  \"departureDate\": \"" << escapeJson(booking.departureDate) << "\",\n"
		<< "  \"departureSlot\": " << booking.departureSlot << ",\n"
		<< "  \"returnFlight\": " << booking.returnFlight << ",\n"
		<< "  \"returnDate\": \"" << escapeJson(booking.returnDate) << "\",\n"
		<< "  \"returnSlot\": " << booking.returnSlot << ",\n"
		<< "  \"passengers\": [\n";
	for (size_t index = 0; index < booking.passengers.size(); ++index) {
		json << passengerJson(booking.passengers[index], "    ");
		if (index + 1 < booking.passengers.size()) json << ',';
		json << '\n';
	}
	json << "  ]\n}\n";
	return writeTextAtomically(bookingsRoot / (booking.bookingId + ".json"), json.str());
}

bool loadPaymentJson(const string& bookingId, PaymentRecord& payment) {
	string json = readAll(paymentsRoot / (bookingId + ".json"));
	if (json.empty()) return false;
	auto storedId = stringField(json, "bookingId");
	auto amount = intField(json, "amount");
	auto status = stringField(json, "status");
	auto checkInStatus = stringField(json, "checkInStatus");
	if (!storedId || !amount || !status || !checkInStatus || *storedId != bookingId) return false;
	payment = { *storedId, *amount, *status, *checkInStatus, {} };
	for (const string& item : objectArray(json, "passengers")) payment.passengers.push_back(parsePassenger(item));
	return true;
}

bool savePaymentJson(const PaymentRecord& payment) {
	ostringstream json;
	json << "{\n"
		<< "  \"bookingId\": \"" << escapeJson(payment.bookingId) << "\",\n"
		<< "  \"amount\": " << payment.amount << ",\n"
		<< "  \"status\": \"" << escapeJson(payment.status) << "\",\n"
		<< "  \"checkInStatus\": \"" << escapeJson(payment.checkInStatus) << "\",\n"
		<< "  \"passengers\": [\n";
	for (size_t index = 0; index < payment.passengers.size(); ++index) {
		json << passengerJson(payment.passengers[index], "    ");
		if (index + 1 < payment.passengers.size()) json << ',';
		json << '\n';
	}
	json << "  ]\n}\n";
	return writeTextAtomically(paymentsRoot / (payment.bookingId + ".json"), json.str());
}

fs::path invoicePathForBooking(const string& bookingId) {
	return invoicesRoot / (bookingId + ".txt");
}
