#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct UserRecord {
	std::string firstName;
	std::string lastName;
	std::string mobile;
	std::string email;
	std::string username;
	std::string password;
};

struct PassengerRecord {
	std::string firstName;
	std::string lastName;
	std::string passport;
	std::string contactFirstName;
	std::string contactLastName;
	std::string contactMobile;
};

struct BookingRecord {
	std::string bookingId;
	std::string username;
	int departureFlight = 0;
	std::string departureDate;
	int departureSlot = 0;
	int returnFlight = 0;
	std::string returnDate;
	int returnSlot = 0;
	std::vector<PassengerRecord> passengers;
};

struct PaymentRecord {
	std::string bookingId;
	int amount = 0;
	std::string status = "Unpaid";
	std::string checkInStatus = "Not Checked";
	std::vector<PassengerRecord> passengers;
};

bool initializeJsonStorage();
std::vector<UserRecord> loadUsersJson();
bool saveUserJson(const UserRecord& user);
std::string bookingIdForUser(const std::string& username);
bool bookingJsonExists(const std::string& username);
bool loadBookingJson(const std::string& username, BookingRecord& booking);
bool saveBookingJson(const BookingRecord& booking);
bool loadPaymentJson(const std::string& bookingId, PaymentRecord& payment);
bool savePaymentJson(const PaymentRecord& payment);
std::filesystem::path invoicePathForBooking(const std::string& bookingId);
bool writeTextAtomically(const std::filesystem::path& path, const std::string& content);
