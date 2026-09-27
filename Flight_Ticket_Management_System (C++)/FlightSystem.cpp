#include "FlightSystem.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

string currentUser = "lily";
string firstNameD = "Lily Lee Li";
string lastNameD = "Ling";

int flightDepartD = 7;
string departDD = "12/08/2025";
int slotDepartD = 1;

int flightReturnD = 8;
string returnDD = "14/08/2025";
int slotReturnD = 3;

int noTickets = 3;

void title() {
	cout << string(SIZE, '-') << '\n';
	cout << "JSJK FLIGHT TICKET MANAGEMENT SYSTEM " << '\n';
	cout << string(SIZE, '-') << '\n';
}

void menu() {
	cout << "1. Book Flight Ticket(s)" << endl;
	cout << "2. Edit Booking" << endl;
	cout << "3. Perform Payment" << endl;
	cout << "4. Check-In Flight" << endl;
	cout << "5. Print Invoice" << endl;
	cout << "6. Quit" << endl;
}

void FlightSchedule() {
	cout << "Flight Available" << endl;
	cout << "1. KL - Penang --> RM200" << endl;
	cout << "2. Penang - KL --> RM200" << endl;
	cout << "3. KL - Johor --> RM200" << endl;
	cout << "4. Johor - KL --> RM200" << endl;
	cout << "5. KL - Singapore --> RM250" << endl;
	cout << "6. Singapore - KL --> RM250" << endl;
	cout << "7. KL - Bangkok --> RM300" << endl;
	cout << "8. Bangkok - KL --> RM300" << endl;

	cout << "Available Departure/Return Time Slots:" << endl;
	cout << "1. 8:00 A.M." << endl;
	cout << "2. 13:00 P.M." << endl;
	cout << "3. 18:00 P.M." << endl;
	cout << "4. 23:00 P.M." << endl;
}

void registration() {
	string firstname, lastname, mobileno, email, username, password, line;
	char choice;
	ifstream checkUserExist("user.txt");
	ofstream addNewUser("user.txt", ios::app);
	if (!checkUserExist.is_open()) {
		cout << "Error in open file. Please try again." << '\n';
		return;
	}
	if (!addNewUser.is_open()) {
		cout << "Error in open file. Please try again." << '\n';
		return;
	}
	cout << "Please key in details for registration:" << '\n';
	cout << "First name: ";
	getline(cin, firstname);
	cout << "Last name: ";
	getline(cin, lastname);
	cout << "Mobile No: ";
	getline(cin, mobileno);
	cout << "Email: ";
	getline(cin, email);
	cout << "Username: ";
	getline(cin, username);
	do {
		cout << "Password (At least 8 characters (1 symbol, 1 uppercase letter, 1 number)):";
		getline(cin, password);
		if (password.length() < 8) {
			cout << "Password less than 8 characters. Please try again." << '\n';
			continue;
		}
		bool Upper = false, Digit = false, Symbol = false;
		for (char ch : password) {
			if (isupper(ch)) Upper = true;
			else if (isdigit(ch)) Digit = true;
			else if (ispunct(ch)) Symbol = true;
		}
		if (!(Upper && Digit && Symbol)) {
			cout << "Password must contain at least 1 uppercase letter, 1 digit, and 1 symbol.\n";
		}
		else {
			break;
		}
	} while (true);

	cout << "Confirm to register? (y-yes,n-no): ";
	cin >> choice;
	choice = tolower(choice);
	cin.ignore();
	if (choice == 'y') {
		bool phonecheck = false, emailcheck = false, usernamecheck = false;
		//Mobile Number Validation
		while (getline(checkUserExist, line)) {
			if (line == mobileno) {
				phonecheck = true;
				break;
			}

		}
		if (phonecheck) {
			cout << "This mobile number have been taken. Please try again." << '\n';
			return;
		}
		checkUserExist.clear();
		checkUserExist.seekg(0);
		//Email Validation
		while (getline(checkUserExist, line)) {
			if (line == email) {
				emailcheck = true;
				break;
			}
		}
		if (emailcheck) {
			cout << "This email have been taken. Please try again." << '\n';
			return;
		}
		checkUserExist.clear();
		checkUserExist.seekg(0);
		//Username Validation
		while (getline(checkUserExist, line)) {
			if (line == username) {
				usernamecheck = true;
				break;
			}
		}
		if (usernamecheck) {
			cout << "This username have been taken. Please try again." << '\n';
			return;
		}
		//Input Saved to "user.txt"
		addNewUser << firstname << '\n';
		addNewUser << lastname << '\n';
		addNewUser << mobileno << '\n';
		addNewUser << email << '\n';
		addNewUser << username << '\n';
		addNewUser << password << '\n';
	}
	else if (choice == 'n') {
		return;
	}
	else {
		cout << "Invalid input. Please try again. . ." << endl;
		system("pause");
	}
	checkUserExist.close();
	addNewUser.close();
}

void readUser(string firstname[], string lastname[], string mobileno[], string email[], string username[], string password[], int& reguser) {
	ifstream readFile("user.txt");
	if (!readFile.is_open()) {
		cout << "Error in open file. Please try again." << '\n';
		return;
	}
	else {
		reguser = 0;
		while (getline(readFile, firstname[reguser]) &&//FIRST NAME
			getline(readFile, lastname[reguser]) &&//LAST NAME
			getline(readFile, mobileno[reguser]) &&//MOBILE NUMBER
			getline(readFile, email[reguser]) &&//EMAIL
			getline(readFile, username[reguser]) &&//USERNAME
			getline(readFile, password[reguser])) {//PASSWORD) 

			reguser++;
		}
		readFile.close();
	}
}

int login(string username[], string password[], int reguser) {
	string userName, passWord;                        //DEFINE VARIABLE
	bool usernameTrue = false, passwordTrue = false; //DEFINE VARIABLE
	int userIndex = -1;                              //DEFINE VARIABLE

	cout << "Username: ";
	getline(cin, userName);							//KEY IN USERNAME
	cout << "Password: ";
	getline(cin, passWord);							//KEY IN PASSWORD

	for (int i = 0; i < reguser; i++) {
		if (username[i] == userName) {					//USERNAME=MATCH
			usernameTrue = true;
			if (password[i] == passWord) {				//USERNAME=MATCH AND PASSWORD=MATCH
				passwordTrue = true;
				userIndex = i;
				break;
			}
		}
		else if (password[i] == passWord) {
			passwordTrue = true;
		}

	}
	if (usernameTrue && passwordTrue) {					//USERNAME=MATCH AND PASSWORD=MATCH
		cout << "Login successfull!" << '\n';
		system("pause");
		currentUser = userName;
		return userIndex;
	}
	else if (usernameTrue && !passwordTrue) {			//USERNAME=MATCH AND PASSWORD/=MATCH
		cout << "Incorrect Password. Please try again" << '\n';
		return -1;


	}
	else if (!usernameTrue && passwordTrue) {			//USERNAME/=MATCH AND PASSWORD=MATCH
		cout << "Username does not exist.Please register an account" << '\n';
		return -2;


	}
	else {												//USERNAME/=MATCH AND PASSWORD/=MATCH
		cout << "Invalid username & password. Please try again." << '\n';
		return -3;

	}

}

void performBooking() {
	int passengernum;
	char confirmation;
	FlightSchedule();
	cout << "Number of passenger(s):" << endl;
	cin >> passengernum;
	cin.ignore();
	if (passengernum <= 0 || passengernum > SIZE) {
		cout << "Invalid number of passengers. Please try again. . ." << endl;
		return;
	}
	//TEMPORARY ARRAY FILE
	string firstnameArr[SIZE], lastnameArr[SIZE], depDateArr[SIZE], retDateArr[SIZE];
	int depFlightArr[SIZE], depSlotArr[SIZE], flightReturnArr[SIZE], retSlotArr[SIZE];
	cout << "Please provide the details as below:" << '\n';
	cout << '\n';
	for (int i = 0; i < passengernum; i++) {
		cout << "Passenger " << i + 1 << '\n';
		cout << "First Name: ";
		getline(cin, firstnameArr[i]);
		cout << "Last Name: ";
		getline(cin, lastnameArr[i]);

		cout << "Departure Flight: ";
		cin >> depFlightArr[i];
		cin.ignore();
		cout << "Date of Departure (DD/MM/YYYY): ";
		getline(cin, depDateArr[i]);
		cout << "Slot of Departure: ";
		cin >> depSlotArr[i];
		cin.ignore();

		cout << "Return Flight: ";
		cin >> flightReturnArr[i];
		cin.ignore();
		cout << "Date of Return (DD/MM/YYYY): ";
		getline(cin, retDateArr[i]);
		cout << "Slot of Return: ";
		cin >> retSlotArr[i];
		cin.ignore();
	}

	cout << "Confirm booking (y-yes,n-no): ";
	cin >> confirmation;
	confirmation = tolower(confirmation);
	cin.ignore();

	if (confirmation == 'y') {
		ofstream bookingFile(currentUser + "_Booking.txt");
		if (!bookingFile.is_open()) {
			cout << "File cannot be open. Please try again." << endl;
			return;
		}
		for (int j = 0; j < passengernum; j++) {
			bookingFile << firstnameArr[j] << '\n';
			bookingFile << lastnameArr[j] << '\n';
			bookingFile << depFlightArr[j] << '\n';
			bookingFile << depDateArr[j] << '\n';
			bookingFile << depSlotArr[j] << '\n';
			bookingFile << flightReturnArr[j] << '\n';
			bookingFile << retDateArr[j] << '\n';
			bookingFile << retSlotArr[j] << '\n';
		}
		bookingFile.close();
		cout << "Booking process successful. Proceeding to Menu." << '\n';
		system("pause");
	}
	else {
		cout << "Booking process cancelled. Proceeding to Menu." << '\n';
		system("pause");
	}

}

void readBooking(string firstname[], string lastname[], int depflight[], string depdate[], int deptime[], int retflight[], string retdate[], int rettime[], int& totpassenger) {
	ifstream readingFile(currentUser + "_Booking.txt");
	totpassenger = 0;

	while (totpassenger < SIZE) {
		if (!getline(readingFile, firstname[totpassenger])) break;
		if (!getline(readingFile, lastname[totpassenger])) break;

		if (!(readingFile >> depflight[totpassenger])) break;
		readingFile.ignore();

		if (!getline(readingFile, depdate[totpassenger])) break;

		if (!(readingFile >> deptime[totpassenger])) break;
		readingFile.ignore();

		if (!(readingFile >> retflight[totpassenger])) break;
		readingFile.ignore();

		if (!getline(readingFile, retdate[totpassenger])) break;

		if (!(readingFile >> rettime[totpassenger])) break;
		readingFile.ignore();

		totpassenger++; // Only increment if full passenger read success
	}
	if (totpassenger > 0) {
		firstNameD = firstname[0];
		lastNameD = lastname[0];
		flightDepartD = depflight[0];
		departDD = depdate[0];
		slotDepartD = deptime[0];

		flightReturnD = retflight[0];
		returnDD = retdate[0];
		slotReturnD = rettime[0];
		noTickets = totpassenger;
	}
	else {

		noTickets = 0;
	}
	readingFile.close();
}

void editBooking(string firstname[], string lastname[], int depflight[], string depdate[], int deptime[], int retflight[], string retdate[], int rettime[], int totpassenger) {
	char editNameChoice, confirm;
	int passengerNum, editChoice, newFlightOrSlot;
	string newDate;
	ifstream checkFile(currentUser + "_Booking.txt");
	if (!checkFile.is_open()) {
		title();
		cout << "No booking available, please book your flight ticket(s)!" << endl;
		system("pause");
		return;
	}
	checkFile.close();

	title();
	FlightSchedule();
	cout << "Change passenger name? (y-yes, n-no): ";
	cin >> editNameChoice;
	editNameChoice = tolower(editNameChoice);
	cin.ignore();

	if (editNameChoice == 'y') {
		cout << "Enter passenger number to change (1 to " << totpassenger << "): ";
		cin >> passengerNum;
		cin.ignore();
		cout << "Enter new First Name: ";
		getline(cin, firstname[passengerNum - 1]);
		cout << "Enter new Last Name: ";
		getline(cin, lastname[passengerNum - 1]);
	}
	else if (editNameChoice == 'n') {
		cout << "Please select item to amend" << endl;
		cout << "1. Departure Flight" << endl;
		cout << "2. Departure Date" << endl;
		cout << "3. Departure Slot" << endl;
		cout << "4. Return Flight" << endl;
		cout << "5. Return Date" << endl;
		cout << "6. Return Slot" << endl;
		cout << "Select: ";
		cin >> editChoice;
		cin.ignore();

		cout << "Follow the details of the 1st Passenger " << firstname[0] << " " << lastname[0] << endl;
		if (editChoice == 1) {
			cout << "Old departure flight: " << flightDepartD << endl;
			cout << "New departure flight: ";
			cin >> newFlightOrSlot;
			cin.ignore();
			for (int i = 0; i < totpassenger; i++) {
				depflight[i] = newFlightOrSlot;
			}

		}
		else if (editChoice == 2) {
			cout << "Old departure date: " << departDD << endl;
			cout << "New departure date: ";
			getline(cin, newDate);
			for (int i = 0; i < totpassenger; i++) {
				depdate[i] = newDate;
			}

		}
		else if (editChoice == 3) {
			cout << "Old departure slot: " << slotDepartD << endl;
			cout << "New departure slot: ";
			cin >> newFlightOrSlot;
			cin.ignore();
			for (int i = 0; i < totpassenger; i++) {
				deptime[i] = newFlightOrSlot;
			}

		}
		else if (editChoice == 4) {
			cout << "Old return flight: " << flightReturnD << endl;
			cout << "New return flight: ";
			cin >> newFlightOrSlot;
			cin.ignore();
			for (int i = 0; i < totpassenger; i++) {
				retflight[i] = newFlightOrSlot;
			}
		}
		else if (editChoice == 5) {
			cout << "Old return date: " << returnDD << endl;
			cout << "New return date: ";
			getline(cin, newDate);
			for (int i = 0; i < totpassenger; i++) {
				retdate[i] = newDate;
			}

		}
		else if (editChoice == 6) {
			cout << "Follow the details of the 1st Passenger " << firstname[0] << " " << lastname[0] << endl;
			cout << "Old return slot: " << slotReturnD << endl;
			cout << "New return slot: ";
			cin >> newFlightOrSlot;
			cin.ignore();
			for (int i = 0; i < totpassenger; i++) {
				rettime[i] = newFlightOrSlot;
			}
		}
		else {
			cout << "Invalid input. Please try again. . ." << endl;
			return;
		}

	}
	else {
		cout << "Invalid input. Please try again. . ." << endl;
		return;
	}
	cout << "Confirm Amendment? (y-yes,n-no): ";
	cin >> confirm;
	confirm = tolower(confirm);
	if (confirm == 'y') {
		ofstream updateFile(currentUser + "_Booking.txt");
		if (!updateFile.is_open()) {
			cout << "Error saving booking." << endl;
			return;
		}
		for (int i = 0; i < totpassenger; i++) {
			updateFile << firstname[i] << endl;
			updateFile << lastname[i] << endl;
			updateFile << depflight[i] << endl;
			updateFile << depdate[i] << endl;
			updateFile << deptime[i] << endl;
			updateFile << retflight[i] << endl;
			updateFile << retdate[i] << endl;
			updateFile << rettime[i] << endl;
		}
		updateFile.close();
	}
	else {
		cout << "Amendment Cancelled..." << endl;
	}

}

void payment() {
	string flightroute, departTime, returnroute, returnTime;
	string cardholname, cardnum, validdate, securcode, bankname;
	char transacconfirm;
	int departprice = 0, returnprice = 0, finalprice = 0, paymentmethod;

	string bookingfilename = currentUser + "_Booking.txt";
	ifstream fileExist(bookingfilename);
	if (!fileExist.is_open()) {
		title();
		cout << "No booking available, please book your flight ticket(s)!" << endl;
		system("pause");
		return;
	}
	fileExist.close();

	string paymentfilename = currentUser + "_paymentCheckIn.txt";
	ofstream paymentFile(paymentfilename);
	if (!paymentFile.is_open()) {
		cout << "File cannot be open. Please try again." << endl;
		return;
	}

	if (flightDepartD == 1) {
		flightroute = "KL - Penang";
		departprice = 200;
	}
	else if (flightDepartD == 2) {
		flightroute = "Penang - KL";
		departprice = 200;
	}
	else if (flightDepartD == 3) {
		flightroute = "KL - Johor";
		departprice = 200;
	}
	else if (flightDepartD == 4) {
		flightroute = "Johor - KL";
		departprice = 200;
	}
	else if (flightDepartD == 5) {
		flightroute = "KL - Singapore";
		departprice = 250;
	}
	else if (flightDepartD == 6) {
		flightroute = "Singapore - KL";
		departprice = 250;
	}
	else if (flightDepartD == 7) {
		flightroute = "KL - Bangkok";
		departprice = 300;
	}
	else if (flightDepartD == 8) {
		flightroute = "Bangkok - KL";
		departprice = 300;
	}

	if (slotDepartD == 1) {
		departTime = "8:00 A.M.";
	}
	else if (slotDepartD == 2) {
		departTime = "13:00 P.M.";
	}
	else if (slotDepartD == 3) {
		departTime = "18:00 P.M.";
	}
	else if (slotDepartD == 4) {
		departTime = "23:00 P.M.";
	}

	//RETURN ROUTE
	if (flightReturnD == 1) {
		returnroute = "KL - Penang";
		returnprice = 200;
	}
	else if (flightReturnD == 2) {
		returnroute = "Penang - KL";
		returnprice = 200;
	}
	else if (flightReturnD == 3) {
		returnroute = "KL - Johor";
		returnprice = 200;
	}
	else if (flightReturnD == 4) {
		returnroute = "Johor - KL";
		returnprice = 200;
	}
	else if (flightReturnD == 5) {
		returnroute = "KL - Singapore";
		returnprice = 250;
	}
	else if (flightReturnD == 6) {
		returnroute = "Singapore - KL";
		returnprice = 250;
	}
	else if (flightReturnD == 7) {
		returnroute = "KL - Bangkok";
		returnprice = 300;
	}
	else if (flightReturnD == 8) {
		returnroute = "Bangkok - KL";
		returnprice = 300;
	}

	//RETURN TIME
	if (slotReturnD == 1) {
		returnTime = "8:00 A.M.";
	}
	else if (slotReturnD == 2) {
		returnTime = "13:00 P.M.";
	}
	else if (slotReturnD == 3) {
		returnTime = "18:00 P.M.";
	}
	else if (slotReturnD == 4) {
		returnTime = "23:00 P.M.";
	}

	cout << "Total Flight Ticket(s): " << noTickets << endl;
	cout << "Departure Flight: " << departDD << ", " << flightroute << ", " << departTime << endl;
	cout << "Return Flight: " << returnDD << ", " << returnroute << ", " << returnTime << endl;
	finalprice = (departprice + returnprice) * noTickets;
	cout << "Total Payment:  (RM " << departprice << " + RM " << returnprice << ") * " << noTickets << " = RM " << finalprice << endl;
	cout << "Please choose your payment method:" << endl;
	cout << "1. Credit card/Debit Card" << endl;
	cout << "2. Bank Transfer" << endl;
	cin >> paymentmethod;

	if (paymentmethod == 1) {//CREDIT/DEBIT CARD
		cout << "Transfer amount: RM" << finalprice << endl;
		cin.ignore();
		cout << "Card Holder Name: ";
		getline(cin, cardholname);

		cout << "Card Number: ";
		getline(cin, cardnum);

		cout << "Card Valid Date (MM/YYYY): ";
		getline(cin, validdate);

		cout << "Security Code: ";
		getline(cin, securcode);


	}
	else if (paymentmethod == 2) {//BANK TRANFER
		cout << "Transfer amount: RM" << finalprice << endl;
		cout << "Bank Name: ";
		cin >> bankname;
		cin.ignore();
		cout << "Card Holder Name: ";
		cin >> cardholname;
		cin.ignore();
		cout << "Card Number: ";
		cin >> cardnum;
		cin.ignore();
		cout << "Card Valid Date (MM/YYYY): ";
		cin >> validdate;
		cin.ignore();
		cout << "Security Code: ";
		cin >> securcode;
		cin.ignore();
	}
	else {
		cout << "Invalid input. Please try again." << endl;
		return;
	}
	cout << "Confirm Transaction? (y-yes, n-no): ";
	cin >> transacconfirm;
	cin.ignore();
	if (transacconfirm == 'y' || transacconfirm == 'Y') {
		paymentFile << finalprice << endl;
		paymentFile << "Paid" << endl;
		for (int i = 1; i <= noTickets; i++) {
			paymentFile << "NULL" << endl;//FIRST NAME
			paymentFile << "NULL" << endl;//LAST NAME
			paymentFile << "NULL" << endl;//PASSPORT NUMBER
			paymentFile << "NULL" << endl;//CONTACT FIRST NAME
			paymentFile << "NULL" << endl;//CONTACT LAST NAME
			paymentFile << "NULL" << endl;//PHONE NUMBER
		}
		paymentFile << "Not Check" << endl;
	}
	else if (transacconfirm == 'n' || transacconfirm == 'N') {
		paymentFile << finalprice << endl;
		paymentFile << "Unpaid" << endl;
	}
	paymentFile.close();
}

void readPaymentCheckIn(int& totpassenger, string& paymentstatus, string firstname[], string lastname[], string passport[], string confirst[], string conlast[], string mobileno[], string& checkinstatus) {
	int amount;
	string line;
	//Read Passengers First Name and Last Name
	ifstream readFirstLastName(currentUser + "_Booking.txt");
	if (!readFirstLastName.is_open()) {
		title();
		cout << "No booking available, please book your flight ticket(s)!" << endl;
		system("pause");
		return;
	}
	for (int j = 0; j < totpassenger; j++) {
		getline(readFirstLastName, firstname[j]);
		getline(readFirstLastName, lastname[j]);
		getline(readFirstLastName, line);//1
		getline(readFirstLastName, line);//2
		getline(readFirstLastName, line);//3
		getline(readFirstLastName, line);//4
		getline(readFirstLastName, line);//5
		getline(readFirstLastName, line);//6
	}
	readFirstLastName.close();

	//Read _paymentCheckIn.txt
	string checkingfilename = currentUser + "_paymentCheckIn.txt";
	ifstream readFile(checkingfilename);


	readFile >> amount;
	readFile.ignore();
	getline(readFile, paymentstatus);
	for (int i = 0; i < totpassenger; i++) {
		getline(readFile, line);
		getline(readFile, line);
		getline(readFile, passport[i]);
		getline(readFile, confirst[i]);
		getline(readFile, conlast[i]);
		getline(readFile, mobileno[i]);
	}
	getline(readFile, checkinstatus);
	readFile.close();
}

void checkIn(int& totpassenger, string& paymentstatus, string firstname[], string lastname[], string passport[], string confirst[], string conlast[], string mobileno[], string& checkinstatus) {
	string line;
	char samecon, checkincon;
	ifstream checkinFile(currentUser + "_paymentCheckIn.txt");
	if (!checkinFile.is_open()) {
		cout << "File not exist. Please try again." << endl;
		system("pause");
		return;
	}


	if (paymentstatus == "Unpaid") {

		cout << "Payment Status: " << paymentstatus << endl;
		cout << "No payment for the flight ticket (s) yet, please proceed to make payment before check in. Thank you!" << endl;
		system("pause");
	}
	else if (paymentstatus == "Paid") {
		cout << "Payment Status: " << paymentstatus << endl;
		cout << "~Proceed to check in~" << endl;

		cout << "Passenger 1" << endl;
		cout << "First Name: " << firstname[0] << endl;
		cout << "Last Name: " << lastname[0] << endl;
		cout << "Passport Number: ";
		getline(cin, passport[0]);
		cout << "Contact Person First Name: ";
		getline(cin, confirst[0]);
		cout << "Contact Person Last Name: ";
		getline(cin, conlast[0]);
		cout << "Contact Person Phone Number: ";
		getline(cin, mobileno[0]);

		for (int i = 1; i < totpassenger; i++) {
			cout << "Passenger " << i + 1 << endl;
			cout << "First Name: " << firstname[i] << endl;
			cout << "Last Name: " << lastname[i] << endl;
			cout << "Passport Number: ";
			getline(cin, passport[i]);
			cout << "Same contact person as previous? (y-yes, n-no): ";
			cin >> samecon;
			cin.ignore();
			if (samecon == 'y') {
				mobileno[i] = mobileno[0];

				cout << "Contact Person First Name: ";
				getline(cin, confirst[i]);
				cout << "Contact Person Last Name: ";
				getline(cin, conlast[i]);
				cout << "Contact Person Phone Number: " << mobileno[i] << endl;
			}
			else if (samecon == 'n') {
				cout << "Contact Person First Name: ";
				getline(cin, confirst[i]);
				cout << "Contact Person Last Name: ";
				getline(cin, conlast[i]);
				cout << "Contact Person Phone Number: ";
				getline(cin, mobileno[i]);

			}

		}
		cout << "Confirm Check in? (y-yes, n-no):";
		cin >> checkincon;
		cin.ignore();
		if (checkincon == 'y') {
			string amount;
			getline(checkinFile, amount);
			checkinFile.close();


			ofstream checkinFilewrite(currentUser + "_paymentCheckIn.txt");
			if (!checkinFilewrite.is_open()) {
				cout << "File does not exists. Please try again." << endl;
				return;

			}
			checkinFilewrite << amount << endl;
			checkinFilewrite << paymentstatus << endl;
			for (int j = 0; j < totpassenger; j++) {
				checkinFilewrite << firstname[j] << endl;
				checkinFilewrite << lastname[j] << endl;
				checkinFilewrite << passport[j] << endl;
				checkinFilewrite << confirst[j] << endl;
				checkinFilewrite << conlast[j] << endl;
				checkinFilewrite << mobileno[j] << endl;
			}
			checkinstatus = "Checked";
			checkinFilewrite << checkinstatus << endl;

			checkinFilewrite.close();

		}
		else if (checkincon == 'n') {
			return;
		}


	}


	system("pause");
}

void printInvoice(string firstname[], string lastname[], string passport[], string confirst[], string conlast[], string mobileno[], int depflight[], string depdate[], int deptime[], int retflight[], string retdate[], int rettime[]) {

	ifstream checkFile(currentUser + "_paymentCheckIn.txt");
	if (!checkFile.is_open()) {
		cout << "No booking available, please book your flight ticket(s)!" << endl;
		system("pause");
		return;
	}
	checkFile.close();

	string routes[] = { "KL - Penang","Penang - KL","KL - Johor","Johor - KL","KL - Singapore","Singapore - KL","KL - Bangkok","Bangkok - KL" };
	string slots[] = { "8:00 A.M.", "13:00 P.M.", "18:00 P.M.", "23:00 P.M." };

	///////////////////////////////////////////
	if (depflight[0] < 1 || depflight[0] > 8 ||
		deptime[0] < 1 || deptime[0] > 4 ||
		retflight[0] < 1 || retflight[0] > 8 ||
		rettime[0] < 1 || rettime[0] > 4)
	{
		cerr << "Error: Invalid flight or time slot data. Cannot generate invoice." << endl;
		cout << "Press Enter to continue...";
		cin.ignore();
		cin.get();
		return;
	}


	ofstream invoiceFile(currentUser + "_invoice.txt");
	if (!invoiceFile.is_open()) {
		cout << "Error opening file. Please try again. . ." << endl;
		return;
	}

	string flightroute = routes[depflight[0] - 1];
	string departTime = slots[deptime[0] - 1];
	string returnroute = routes[retflight[0] - 1];
	string returnTime = slots[rettime[0] - 1];

	invoiceFile << "Welcome to JSJK Airline Company" << endl;
	invoiceFile << "Departure Flight: " << flightroute << endl;
	invoiceFile << "Departure Date: " << depdate[0] << endl;
	invoiceFile << "Departure Slot: " << departTime << endl;
	invoiceFile << "Return Flight: " << returnroute << endl;
	invoiceFile << "Return Date: " << retdate[0] << endl;
	invoiceFile << "Return Slot: " << returnTime << endl;
	invoiceFile << endl;

	for (int i = 0; i < noTickets; i++) {
		invoiceFile << "Passenger " << i + 1 << endl;
		invoiceFile << "Name: " << firstname[i] << " " << lastname[i] << endl;
		invoiceFile << "Passport Number: " << passport[i] << endl;
		invoiceFile << "Contact Person: " << confirst[i] << " " << conlast[i] << endl;
		invoiceFile << "Contact Person Mobile No: " << mobileno[i] << endl;
		invoiceFile << endl;
	}

	cout << "Invoice printed, please check your folder!" << endl;
	invoiceFile.close();
	cout << "Press Enter to continue...";
	cin.ignore();
	cin.get();
}

