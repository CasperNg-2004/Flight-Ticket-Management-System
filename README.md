# Flight Ticket Management System

A console-based flight ticket management system written in C++20. The application supports account registration, authentication, flight booking, booking amendments, payment recording, passenger check-in, and invoice generation.

Application data is stored as JSON under a dedicated `data` directory. Records are written through temporary files and atomically replaced to reduce the risk of corrupting existing data.

## Features

- Register and log in to user accounts
- Validate password requirements and duplicate account details
- View available routes, prices, and departure slots
- Book tickets for one or more passengers
- Validate flight selections, time slots, passenger counts, and travel dates
- Edit unpaid bookings
- Record simulated card or bank-transfer payments
- Prevent duplicate payments and changes to paid bookings
- Check in passengers and record passport and contact information
- Generate text invoices for paid bookings
- Migrate legacy accounts from `user.txt` to JSON on first run

## Technology

- C++20
- Microsoft Visual C++
- Visual Studio/MSBuild
- Standard C++ library
- Windows API for atomic file replacement
- JSON files for persistence

The application is intentionally UI-less and runs in a Windows console.

## Project structure

```text
Flight_Ticket_Management_System (C++)/
├── README.md
├── Flight_Ticket_Management_System (C++).slnx
└── Flight_Ticket_Management_System (C++)/
    ├── main.cpp
    ├── FlightSystem.h
    ├── FlightSystem.cpp
    ├── JsonStorage.h
    ├── JsonStorage.cpp
    ├── user.txt
    └── Flight_Ticket_Management_System (C++).vcxproj
```

### Source responsibilities

| File | Responsibility |
| --- | --- |
| `main.cpp` | Application entry point and console menu flow |
| `FlightSystem.h` | Public declarations used by the application |
| `FlightSystem.cpp` | Registration, login, booking, payment, check-in, invoice, and validation logic |
| `JsonStorage.h` | Persistent record models and storage declarations |
| `JsonStorage.cpp` | JSON serialization, parsing, migration, stable IDs, and atomic writes |
| `user.txt` | Legacy account data used only for first-run migration |

## Requirements

- Windows 10 or later
- Visual Studio with the **Desktop development with C++** workload
- A compatible MSVC toolset and Windows SDK
- C++20 support

The supplied project currently targets the `v145` platform toolset. If it is unavailable, open the project properties and select an installed MSVC platform toolset.

## Building in Visual Studio

1. Open `Flight_Ticket_Management_System (C++).slnx` in Visual Studio.
2. Select `Debug` or `Release` and the `x64` platform.
3. Choose **Build → Build Solution**.
4. Run the project with **Debug → Start Without Debugging**.

The Debug x64 executable is normally produced at:

```text
x64/Debug/Flight_Ticket_Management_System (C++).exe
```

## Using the application

The initial menu provides three options:

```text
1. Register New User
2. Login
3. Quit
```

After logging in, a user can:

```text
1. Book Flight Ticket(s)
2. Edit Booking
3. Perform Payment
4. Check-In Flight
5. Print Invoice
6. Quit
```

### Typical workflow

1. Register an account or log in with an existing account.
2. Create a booking and enter passenger details.
3. Select departure and return flights, dates, and time slots.
4. Amend the booking if required.
5. Confirm the simulated payment.
6. Check in each passenger.
7. Generate the invoice.

## Available flights

| Number | Route | One-way price |
| ---: | --- | ---: |
| 1 | KL → Penang | RM200 |
| 2 | Penang → KL | RM200 |
| 3 | KL → Johor | RM200 |
| 4 | Johor → KL | RM200 |
| 5 | KL → Singapore | RM250 |
| 6 | Singapore → KL | RM250 |
| 7 | KL → Bangkok | RM300 |
| 8 | Bangkok → KL | RM300 |

Available time slots are 8:00 AM, 1:00 PM, 6:00 PM, and 11:00 PM.

## Input validation

The system enforces the following rules:

- Menu choices must fall within the displayed range.
- A booking must contain between 1 and 100 passengers.
- Flight numbers must be between 1 and 8.
- Time slots must be between 1 and 4.
- Dates must be valid and use `DD/MM/YYYY` format.
- The return date cannot precede the departure date.
- Required text fields cannot be empty.
- Passenger numbers must refer to an existing passenger.
- Passwords must contain at least eight characters, one uppercase letter, one number, and one symbol.

## Data storage

The application creates the following directories at runtime:

```text
data/
├── users/
│   └── usr_<stable-id>.json
├── bookings/
│   └── bkg_<stable-id>.json
├── payments/
│   └── bkg_<stable-id>.json
└── invoices/
    └── bkg_<stable-id>.txt
```

Usernames are not used directly as filenames. Deterministic stable identifiers associate each user with a booking while preventing path characters in usernames from affecting storage locations.

All JSON and invoice writes are first made to a temporary file. The temporary file replaces the destination only after the write succeeds.

### Legacy migration

If `user.txt` exists and `data/users` does not contain any users, the application imports the legacy records into JSON automatically. The legacy file is not deleted.

The previous username-based booking and payment text files are not automatically migrated.

## Payment behavior

Payment processing is simulated; the application does not connect to a payment provider.

- Card numbers and security codes are not requested or stored.
- Cancelling payment leaves existing storage unchanged.
- A paid booking cannot be paid again, edited, or replaced.
- Check-in and invoice generation require a paid booking.

## Security notice

This is an educational project, not a production reservation or payment system.

Passwords are currently stored in plaintext JSON so that existing authentication behavior remains compatible. Do not use real passwords or personal information. A future security improvement should replace plaintext passwords with a password-hashing algorithm designed for authentication, such as Argon2id, bcrypt, or scrypt.

## Current limitations

- One active booking is supported per username.
- Flight schedules and prices are compiled into the application.
- Payment is simulated.
- Storage is local and intended for a single running application instance.
- The application is Windows-specific because atomic replacement uses the Windows API.
- There is no automated test project yet.

## Suggested future improvements

- Hash passwords and remove legacy plaintext account data.
- Replace parallel arrays with domain classes throughout the application.
- Add automated unit and workflow tests.
- Support multiple bookings and booking history per user.
- Move flight schedules and prices into structured configuration data.
- Add cancellation and refund workflows.
- Improve error reporting for corrupted JSON records.

## License

No license has been specified for this project.
