# tty-login

A minimal Unix-style login system written in C.

This project demonstrates low-level terminal handling using `termios`, 
raw input mode, signal handling with `sigaction`, and masked password input.

⚠️ This is an educational project. Passwords are stored in plaintext and 
are NOT secure for real-world usage.

---

## ✨ Features

- Raw terminal mode (non-canonical input)
- Disabled input echo for password masking
- Signal-safe input handling (SIGINT support)
- Backspace support
- EOF (Ctrl+D) handling
- Simple credential verification from file

---

## 🛠 How It Works

- Uses `tcgetattr()` and `tcsetattr()` to switch terminal into raw mode.
- Disables `ICANON` and `ECHO` flags.
- Reads input character-by-character using `read()`.
- Masks password characters with `*`.
- Handles `SIGINT` safely using `sigaction`.
- Verifies credentials from `data.txt`.

---