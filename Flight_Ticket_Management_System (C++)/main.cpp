#include "FlightSystem.h"
#include "JsonStorage.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main() {
	if (!initializeJsonStorage()) {
		cerr << "Unable to initialize the data directory.\n";
		return 1;
	}

	int contRL = 1, choice, userCount = 0, menuchoice, contRL2 = 1, totbooking = 0;
	string paymentStatus, checkinStatus;
	string firstName[SIZE], lastName[SIZE], mobileNum[SIZE], email[SIZE], userName[SIZE], passWord[SIZE];
	string passPort[SIZE], conFirst[SIZE], conLast[SIZE], mobileno[SIZE];
	string firstNameBook[SIZE], lastNameBook[SIZE], depDate[SIZE], retDate[SIZE];
	int depFlights[SIZE], depSlots[SIZE], retFlights[SIZE], retSlots[SIZE];

	do {
		title();
		cout << "1. Register New User " << '\n';
		cout << "2. Login" << '\n';
		cout << "3. Quit" << '\n';
		choice = readIntInRange("Choice: ", 1, 3);

		if (choice == 1) {
			registration();
			contRL = readIntInRange("Do you want to continue login? (1-yes, 2-no): ", 1, 2);
			if (contRL == 2) {
				return 0;
			}
		}
		else if (choice == 2) {
			readUser(firstName, lastName, mobileNum, email, userName, passWord, userCount);
			int userIndex = login(userName, passWord, userCount);
			if (userIndex >= 0) {
				system("cls");
				do {
					title();
					cout << "Username: " << currentUser << '\n';
					menu();
					menuchoice = readIntInRange("Choice: ", 1, 6);
					system("cls");

					if (menuchoice == 1) {
						title();
						performBooking();
					}
					else if (menuchoice == 2) {
						readBooking(firstNameBook, lastNameBook, depFlights, depDate, depSlots, retFlights, retDate, retSlots, totbooking);
						editBooking(firstNameBook, lastNameBook, depFlights, depDate, depSlots, retFlights, retDate, retSlots, totbooking);
					}
					else if (menuchoice == 3) {
						title();
						readBooking(firstNameBook, lastNameBook, depFlights, depDate, depSlots, retFlights, retDate, retSlots, totbooking);
						payment();
					}
					else if (menuchoice == 4) {
						title();
						readPaymentCheckIn(totbooking, paymentStatus, firstNameBook, lastNameBook, passPort, conFirst, conLast, mobileno, checkinStatus);
						checkIn(totbooking, paymentStatus, firstNameBook, lastNameBook, passPort, conFirst, conLast, mobileno, checkinStatus);
					}
					else if (menuchoice == 5) {
						readBooking(firstNameBook, lastNameBook, depFlights, depDate, depSlots, retFlights, retDate, retSlots, totbooking);
						readPaymentCheckIn(totbooking, paymentStatus, firstNameBook, lastNameBook, passPort, conFirst, conLast, mobileno, checkinStatus);
						title();
						printInvoice(firstNameBook, lastNameBook, passPort, conFirst, conLast, mobileno, depFlights, depDate, depSlots, retFlights, retDate, retSlots);
					}
					else if (menuchoice == 6) {
						break;
					}
					else {
						cout << "Invalid input. Please try again. . ." << endl;
						system("pause");
					}

					system("cls");
				} while (contRL2 == 1);
			}
			else {
				contRL = readIntInRange("Do you want to continue login? (1-yes, 2-no): ", 1, 2);
				if (contRL == 2) {
					return 0;
				}
			}
		}
		else if (choice == 3) {
			break;
		}
		else {
			cout << "Invalid input. Please try again. . ." << endl;
			system("pause");
		}
		system("cls");
	} while (contRL == 1);

	return 0;
}
