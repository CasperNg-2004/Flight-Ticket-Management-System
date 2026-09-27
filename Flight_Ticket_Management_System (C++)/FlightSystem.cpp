#include "FlightSystem.h"
#include "JsonStorage.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string currentUser;
string firstNameD, lastNameD, departDD, returnDD;
int flightDepartD = 0, slotDepartD = 0, flightReturnD = 0, slotReturnD = 0, noTickets = 0;

namespace {
const string routes[] = { "KL - Penang", "Penang - KL", "KL - Johor", "Johor - KL", "KL - Singapore", "Singapore - KL", "KL - Bangkok", "Bangkok - KL" };
const string slots[] = { "8:00 A.M.", "13:00 P.M.", "18:00 P.M.", "23:00 P.M." };
const int prices[] = { 200, 200, 200, 200, 250, 250, 300, 300 };

string requiredLine(const string& prompt) {
	while (true) {
		cout << prompt;
		string value;
		getline(cin, value);
		if (!value.empty()) return value;
		cout << "This field cannot be empty.\n";
	}
}

char yesNo(const string& prompt) {
	while (true) {
		string value = requiredLine(prompt);
		char choice = static_cast<char>(tolower(static_cast<unsigned char>(value[0])));
		if (value.size() == 1 && (choice == 'y' || choice == 'n')) return choice;
		cout << "Please enter y or n.\n";
	}
}

bool parseDate(const string& value, chrono::year_month_day& result) {
	if (value.size() != 10 || value[2] != '/' || value[5] != '/') return false;
	for (size_t i : { 0u, 1u, 3u, 4u, 6u, 7u, 8u, 9u })
		if (!isdigit(static_cast<unsigned char>(value[i]))) return false;
	try {
		result = chrono::year{ stoi(value.substr(6, 4)) } /
			chrono::month{ static_cast<unsigned>(stoi(value.substr(3, 2))) } /
			chrono::day{ static_cast<unsigned>(stoi(value.substr(0, 2))) };
		return result.ok();
	}
	catch (...) { return false; }
}

string validDate(const string& prompt) {
	while (true) {
		string value = requiredLine(prompt);
		chrono::year_month_day parsed;
		if (parseDate(value, parsed)) return value;
		cout << "Enter a valid date in DD/MM/YYYY format.\n";
	}
}

bool validDateOrder(const string& departure, const string& returning) {
	chrono::year_month_day first, second;
	return parseDate(departure, first) && parseDate(returning, second) && chrono::sys_days(second) >= chrono::sys_days(first);
}

void updateGlobals(const BookingRecord& booking) {
	noTickets = static_cast<int>(booking.passengers.size());
	if (booking.passengers.empty()) return;
	firstNameD = booking.passengers[0].firstName;
	lastNameD = booking.passengers[0].lastName;
	flightDepartD = booking.departureFlight;
	departDD = booking.departureDate;
	slotDepartD = booking.departureSlot;
	flightReturnD = booking.returnFlight;
	returnDD = booking.returnDate;
	slotReturnD = booking.returnSlot;
}

bool isPaid(const string& username) {
	PaymentRecord paymentRecord;
	return loadPaymentJson(bookingIdForUser(username), paymentRecord) && paymentRecord.status == "Paid";
}
}

int readIntInRange(const string& prompt, int minimum, int maximum) {
	while (true) {
		cout << prompt;
		string input;
		getline(cin, input);
		try {
			size_t used = 0;
			int value = stoi(input, &used);
			if (used == input.size() && value >= minimum && value <= maximum) return value;
		}
		catch (...) {}
		cout << "Enter a number from " << minimum << " to " << maximum << ".\n";
	}
}

void title() {
	cout << string(SIZE, '-') << "\nJSJK FLIGHT TICKET MANAGEMENT SYSTEM\n" << string(SIZE, '-') << '\n';
}

void menu() {
	cout << "1. Book Flight Ticket(s)\n2. Edit Booking\n3. Perform Payment\n4. Check-In Flight\n5. Print Invoice\n6. Quit\n";
}

void FlightSchedule() {
	cout << "Flight Available\n";
	for (int i = 0; i < 8; ++i) cout << i + 1 << ". " << routes[i] << " --> RM" << prices[i] << '\n';
	cout << "Available Departure/Return Time Slots:\n";
	for (int i = 0; i < 4; ++i) cout << i + 1 << ". " << slots[i] << '\n';
}

void registration() {
	UserRecord user;
	cout << "Please key in details for registration:\n";
	user.firstName = requiredLine("First name: ");
	user.lastName = requiredLine("Last name: ");
	user.mobile = requiredLine("Mobile No: ");
	user.email = requiredLine("Email: ");
	user.username = requiredLine("Username: ");
	while (true) {
		user.password = requiredLine("Password (at least 8 characters, 1 symbol, 1 uppercase letter, 1 number): ");
		bool upper = false, digit = false, symbol = false;
		for (unsigned char ch : user.password) { upper |= !!isupper(ch); digit |= !!isdigit(ch); symbol |= !!ispunct(ch); }
		if (user.password.size() >= 8 && upper && digit && symbol) break;
		cout << "The password does not meet the requirements.\n";
	}
	for (const UserRecord& existing : loadUsersJson()) {
		if (existing.mobile == user.mobile) { cout << "This mobile number has been taken.\n"; return; }
		if (existing.email == user.email) { cout << "This email has been taken.\n"; return; }
		if (existing.username == user.username) { cout << "This username has been taken.\n"; return; }
	}
	if (yesNo("Confirm registration? (y-yes, n-no): ") == 'y')
		cout << (saveUserJson(user) ? "Registration successful.\n" : "Unable to save the user.\n");
}

void readUser(string firstname[], string lastname[], string mobileno[], string email[], string username[], string password[], int& count) {
	vector<UserRecord> users = loadUsersJson();
	count = min(static_cast<int>(users.size()), SIZE);
	for (int i = 0; i < count; ++i) {
		firstname[i] = users[i].firstName; lastname[i] = users[i].lastName; mobileno[i] = users[i].mobile;
		email[i] = users[i].email; username[i] = users[i].username; password[i] = users[i].password;
	}
	if (users.size() > SIZE) cerr << "Only the first " << SIZE << " users can be loaded.\n";
}

int login(string username[], string password[], int count) {
	string enteredUser = requiredLine("Username: ");
	string enteredPassword = requiredLine("Password: ");
	for (int i = 0; i < count; ++i) {
		if (username[i] != enteredUser) continue;
		if (password[i] != enteredPassword) { cout << "Incorrect password.\n"; return -1; }
		currentUser = enteredUser;
		cout << "Login successful!\n";
		return i;
	}
	cout << "Username does not exist.\n";
	return -1;
}

void performBooking() {
	if (isPaid(currentUser)) { cout << "A paid booking already exists and cannot be replaced.\n"; return; }
	FlightSchedule();
	BookingRecord booking;
	booking.bookingId = bookingIdForUser(currentUser);
	booking.username = currentUser;
	int count = readIntInRange("Number of passengers: ", 1, SIZE);
	for (int i = 0; i < count; ++i) {
		cout << "Passenger " << i + 1 << '\n';
		PassengerRecord passenger;
		passenger.firstName = requiredLine("First Name: ");
		passenger.lastName = requiredLine("Last Name: ");
		booking.passengers.push_back(passenger);
	}
	booking.departureFlight = readIntInRange("Departure Flight: ", 1, 8);
	booking.departureDate = validDate("Date of Departure (DD/MM/YYYY): ");
	booking.departureSlot = readIntInRange("Slot of Departure: ", 1, 4);
	booking.returnFlight = readIntInRange("Return Flight: ", 1, 8);
	do {
		booking.returnDate = validDate("Date of Return (DD/MM/YYYY): ");
		if (!validDateOrder(booking.departureDate, booking.returnDate)) cout << "Return date cannot be before departure.\n";
	} while (!validDateOrder(booking.departureDate, booking.returnDate));
	booking.returnSlot = readIntInRange("Slot of Return: ", 1, 4);
	if (yesNo("Confirm booking (y-yes, n-no): ") == 'y') {
		if (saveBookingJson(booking)) { updateGlobals(booking); cout << "Booking successful. Booking ID: " << booking.bookingId << '\n'; }
		else cout << "Unable to save the booking.\n";
	}
}

void readBooking(string firstname[], string lastname[], int depflight[], string depdate[], int deptime[], int retflight[], string retdate[], int rettime[], int& total) {
	BookingRecord booking;
	total = 0;
	if (!loadBookingJson(currentUser, booking)) { noTickets = 0; return; }
	total = min(static_cast<int>(booking.passengers.size()), SIZE);
	for (int i = 0; i < total; ++i) {
		firstname[i] = booking.passengers[i].firstName; lastname[i] = booking.passengers[i].lastName;
		depflight[i] = booking.departureFlight; depdate[i] = booking.departureDate; deptime[i] = booking.departureSlot;
		retflight[i] = booking.returnFlight; retdate[i] = booking.returnDate; rettime[i] = booking.returnSlot;
	}
	updateGlobals(booking);
}

void editBooking(string[], string[], int[], string[], int[], int[], string[], int[], int total) {
	BookingRecord booking;
	if (!loadBookingJson(currentUser, booking) || total <= 0) { cout << "No booking available.\n"; return; }
	if (isPaid(currentUser)) { cout << "Paid bookings cannot be edited.\n"; return; }
	FlightSchedule();
	if (yesNo("Change passenger name? (y-yes, n-no): ") == 'y') {
		int passenger = readIntInRange("Passenger number: ", 1, total) - 1;
		booking.passengers[passenger].firstName = requiredLine("New First Name: ");
		booking.passengers[passenger].lastName = requiredLine("New Last Name: ");
	}
	else {
		cout << "1. Departure Flight\n2. Departure Date\n3. Departure Slot\n4. Return Flight\n5. Return Date\n6. Return Slot\n";
		int choice = readIntInRange("Select: ", 1, 6);
		if (choice == 1) booking.departureFlight = readIntInRange("New departure flight: ", 1, 8);
		else if (choice == 2) booking.departureDate = validDate("New departure date: ");
		else if (choice == 3) booking.departureSlot = readIntInRange("New departure slot: ", 1, 4);
		else if (choice == 4) booking.returnFlight = readIntInRange("New return flight: ", 1, 8);
		else if (choice == 5) booking.returnDate = validDate("New return date: ");
		else booking.returnSlot = readIntInRange("New return slot: ", 1, 4);
		if (!validDateOrder(booking.departureDate, booking.returnDate)) { cout << "Invalid date order; changes were not saved.\n"; return; }
	}
	if (yesNo("Confirm amendment? (y-yes, n-no): ") == 'y') {
		cout << (saveBookingJson(booking) ? "Booking updated.\n" : "Unable to update booking.\n");
		updateGlobals(booking);
	}
}

void payment() {
	BookingRecord booking;
	if (!loadBookingJson(currentUser, booking)) { cout << "No booking available.\n"; return; }
	PaymentRecord oldPayment;
	if (loadPaymentJson(booking.bookingId, oldPayment) && oldPayment.status == "Paid") { cout << "This booking has already been paid.\n"; return; }
	int total = (prices[booking.departureFlight - 1] + prices[booking.returnFlight - 1]) * static_cast<int>(booking.passengers.size());
	cout << "Booking ID: " << booking.bookingId << "\nDeparture: " << booking.departureDate << ", " << routes[booking.departureFlight - 1]
		<< ", " << slots[booking.departureSlot - 1] << "\nReturn: " << booking.returnDate << ", " << routes[booking.returnFlight - 1]
		<< ", " << slots[booking.returnSlot - 1] << "\nTotal Payment: RM " << total << '\n';
	int method = readIntInRange("Payment method (1-card, 2-bank transfer): ", 1, 2);
	cout << "Selected: " << (method == 1 ? "Card" : "Bank transfer") << ". Payment details are not stored.\n";
	if (yesNo("Confirm transaction? (y-yes, n-no): ") == 'n') { cout << "Payment cancelled; existing data was not changed.\n"; return; }
	PaymentRecord record{ booking.bookingId, total, "Paid", "Not Checked", booking.passengers };
	cout << (savePaymentJson(record) ? "Payment recorded successfully.\n" : "Unable to save payment.\n");
}

void readPaymentCheckIn(int& total, string& status, string firstname[], string lastname[], string passport[], string confirst[], string conlast[], string mobile[], string& checkin) {
	BookingRecord booking;
	PaymentRecord paymentRecord;
	total = 0; status.clear(); checkin.clear();
	if (!loadBookingJson(currentUser, booking)) return;
	total = min(static_cast<int>(booking.passengers.size()), SIZE);
	if (!loadPaymentJson(booking.bookingId, paymentRecord)) return;
	status = paymentRecord.status; checkin = paymentRecord.checkInStatus;
	for (int i = 0; i < total; ++i) {
		const PassengerRecord& source = i < paymentRecord.passengers.size() ? paymentRecord.passengers[i] : booking.passengers[i];
		firstname[i] = booking.passengers[i].firstName; lastname[i] = booking.passengers[i].lastName;
		passport[i] = source.passport; confirst[i] = source.contactFirstName; conlast[i] = source.contactLastName; mobile[i] = source.contactMobile;
	}
}

void checkIn(int& total, string& status, string[], string[], string[], string[], string[], string[], string& checkin) {
	BookingRecord booking;
	PaymentRecord record;
	if (!loadBookingJson(currentUser, booking) || !loadPaymentJson(booking.bookingId, record)) { cout << "Complete booking and payment before check-in.\n"; return; }
	if (record.status != "Paid") { cout << "Payment is required before check-in.\n"; return; }
	if (record.checkInStatus == "Checked") { cout << "This booking is already checked in.\n"; return; }
	total = min(static_cast<int>(booking.passengers.size()), SIZE);
	for (int i = 0; i < total; ++i) {
		PassengerRecord& passenger = booking.passengers[i];
		cout << "Passenger " << i + 1 << ": " << passenger.firstName << ' ' << passenger.lastName << '\n';
		passenger.passport = requiredLine("Passport Number: ");
		if (i > 0 && yesNo("Same contact person as passenger 1? (y-yes, n-no): ") == 'y') {
			passenger.contactFirstName = booking.passengers[0].contactFirstName;
			passenger.contactLastName = booking.passengers[0].contactLastName;
			passenger.contactMobile = booking.passengers[0].contactMobile;
		}
		else {
			passenger.contactFirstName = requiredLine("Contact First Name: ");
			passenger.contactLastName = requiredLine("Contact Last Name: ");
			passenger.contactMobile = requiredLine("Contact Phone Number: ");
		}
	}
	if (yesNo("Confirm check-in? (y-yes, n-no): ") == 'n') return;
	record.passengers = booking.passengers; record.checkInStatus = "Checked";
	if (savePaymentJson(record)) { status = record.status; checkin = record.checkInStatus; cout << "Check-in successful.\n"; }
	else cout << "Unable to save check-in.\n";
}

void printInvoice(string[], string[], string[], string[], string[], string[], int[], string[], int[], int[], string[], int[]) {
	BookingRecord booking;
	PaymentRecord record;
	if (!loadBookingJson(currentUser, booking) || !loadPaymentJson(booking.bookingId, record) || record.status != "Paid") {
		cout << "A paid booking is required before an invoice can be generated.\n"; return;
	}
	ostringstream invoice;
	invoice << "Welcome to JSJK Airline Company\nBooking ID: " << booking.bookingId << "\nPayment Status: " << record.status
		<< "\nAmount Paid: RM " << record.amount << "\nDeparture Flight: " << routes[booking.departureFlight - 1]
		<< "\nDeparture Date: " << booking.departureDate << "\nDeparture Slot: " << slots[booking.departureSlot - 1]
		<< "\nReturn Flight: " << routes[booking.returnFlight - 1] << "\nReturn Date: " << booking.returnDate
		<< "\nReturn Slot: " << slots[booking.returnSlot - 1] << "\n\n";
	for (size_t i = 0; i < booking.passengers.size(); ++i) {
		const PassengerRecord& details = i < record.passengers.size() ? record.passengers[i] : booking.passengers[i];
		invoice << "Passenger " << i + 1 << "\nName: " << booking.passengers[i].firstName << ' ' << booking.passengers[i].lastName
			<< "\nPassport Number: " << details.passport << "\nContact Person: " << details.contactFirstName << ' ' << details.contactLastName
			<< "\nContact Mobile: " << details.contactMobile << "\n\n";
	}
	filesystem::path path = invoicePathForBooking(booking.bookingId);
	cout << (writeTextAtomically(path, invoice.str()) ? "Invoice created at " + path.string() + "\n" : "Unable to create invoice.\n");
}
