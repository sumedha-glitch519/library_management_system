
# Library Management System

## 1. Project Overview

This is a console-based **Library Management System** written in C. It manages
three real-world entities — **Students**, **Books**, and **Transactions**
(issue/return/undo) — using four different core data structures, one per job
they are best suited for. The system supports adding, searching, updating and
removing books/students, issuing and returning books, tracking per-student
borrowing history, calculating overdue fines, and undoing the most recent
transaction.

---

## 2. Data Structures Used

| Entity | Data Structure | Why this structure |
|---|---|---|
| Students | Singly Linked List | Students are added/removed dynamically and are usually looked up one at a time by ID; a list keeps insertion O(1) at the point found and avoids needing to pre-size an array. |
| Books | Binary Search Tree (BST), keyed by Book ID | Books need to support fast lookup, insertion and deletion by ID, and an ordered (in-order) listing. A BST gives average O(log n) search/insert/delete and free in-order traversal. |
| Borrowed Books (per student) | Singly Linked List | Each student borrows a variable, small number of books at a time — a lightweight list attached to each `Student` node is ideal. |
| Borrowing History (per student) | Singly Linked List | An append-only log of every issue/return event and fine incurred, kept per student. |
| Transactions (Undo feature) | Singly Linked List used as a **Stack** (LIFO) | Only the *most recent* transaction needs to be undone, and new transactions always need to be undone before older ones — a stack (push/pop from `top`) models this perfectly. |

### 2.1 Student Linked List

```c
struct Student {
    int id;
    char name[50];
    char department[30];
    struct BorrowedBook *borrowedBooks;  // list of currently held books
    struct History      *history;        // log of past actions
    struct Student       *next;
};
```
`head` points to the first student. Traversal is a simple `while (temp != NULL)` walk.

### 2.2 Book Binary Search Tree

```c
struct Book {
    int id;
    char title[100], author[50], category[30];
    int totalCopies, availableCopies;
    struct Book *left, *right;
};
```
`root` points to the top of the tree. The BST is ordered strictly by **Book ID**:
- Left subtree → smaller IDs
- Right subtree → larger IDs

### 2.3 Borrowed Book List (per student)

```c
struct BorrowedBook {
    int bookId;
    int dueDate;
    struct BorrowedBook *next;
};
```
Attached to each `Student`; a book is added here on issue and removed here on return.

### 2.4 History List (per student)

```c
struct History {
    int bookId;
    char action[20];   // "Issued" or "Returned"
    int date;
    float fine;
    struct History *next;
};
```
Append-only — grows every time a book is issued or returned.

### 2.5 Transaction Stack (Undo)

```c
struct Transaction {
    char type[20];       // "issue" or "return"
    int bookId, studentId;
    int dueDate, transactionDate;
    float fine;
    struct Transaction *next;
};
```
`top` is the stack pointer. `pushTransaction()` inserts at the head (push);
`undoLastTransaction()` reads and removes the head (pop) — classic O(1) stack
operations using a linked list, so no fixed array size is needed.

---

## 3. Algorithms Used (per DSA operation)

### 3.1 Linked List Algorithms (Students / Borrowed Books / History)

| Operation | Function | Approach | Time Complexity |
|---|---|---|---|
| Insert (at end) | `addStudent`, `addBorrowedBook`, `addHistory` | Walk to the last node (`next == NULL`) and attach new node | O(n) |
| Search | `findStudent`, `hasBorrowedBook` | Linear scan comparing IDs | O(n) |
| Delete | `removeStudent`, `removeBorrowedBook` | Track `prev` and `temp`, unlink the matching node, `free()` it | O(n) |
| Display all | `displayAllStudents`, `displayBorrowedBooks`, `displayHistory` | Sequential traversal, print each node | O(n) |

### 3.2 Binary Search Tree Algorithms (Books)

| Operation | Function | Approach | Time Complexity (avg / worst) |
|---|---|---|---|
| Insert | `insertBook` (recursive) | Compare new ID with current node; recurse left if smaller, right if larger, until a `NULL` spot is found | O(log n) / O(n) |
| Search | `findBookById` (recursive) | Same comparison logic as insert, but stops and returns the node when `id == root->id` | O(log n) / O(n) |
| In-order traversal (sorted display) | `displayAllBooks` | Recursively visit **left → node → right**, which yields IDs in ascending order automatically | O(n) |
| Search by Title/Author/Category | `searchByTitle`, `searchByAuthor`, `searchByCategory` | Full in-order traversal comparing each node's field with `equalIgnoreCase()`, since these fields are unordered relative to the BST's key (ID) | O(n) |
| Delete | `deleteBook` (recursive) | Classic BST deletion with 3 cases:<br>1. **Leaf node** → simply free it.<br>2. **One child** → replace node with its only child.<br>3. **Two children** → find the **in-order successor** (`findMin` of right subtree), copy its data into the current node, then recursively delete that successor from the right subtree. | O(log n) / O(n) |
| Find minimum | `findMin` | Follow `left` pointers until `left == NULL` — used to find the in-order successor during two-child deletion | O(log n) / O(n) |

### 3.3 Stack Algorithm (Undo)

| Operation | Function | Approach | Time Complexity |
|---|---|---|---|
| Push | `pushTransaction` | Create a node, set `next = top`, then `top = newNode` | O(1) |
| Pop / Undo | `undoLastTransaction` | Read `top`'s data, reverse its effect (see below), then `top = top->next` and `free()` the old top | O(1) |

**Undo logic:**
- If the last transaction was an **issue**, undoing it removes the book from the
  student's borrowed list and increments `availableCopies` (as if it was never issued).
- If the last transaction was a **return**, undoing it decrements `availableCopies`
  (if any copy is free) and re-adds the book to the student's borrowed list with
  its original due date (as if it was never returned).

### 3.4 Date & Fine Calculation Algorithm

- Dates are stored as integers in `YYYYMMDD` format (e.g., `20250115`).
- `convertToTime()` decodes this integer into a `struct tm` and converts it to a
  `time_t` using `mktime()`.
- `daysBetween()` calls `difftime()` on two `time_t` values and divides the
  result (in seconds) by `60 * 60 * 24` to get whole days.
- `calculateFine()` applies a flat rate: **Rs. 5 per day late**, and returns
  `0` if the book was returned on or before the due date.

```
fine = (returnDate − dueDate in days) × 5.0   [if positive, else 0]
```

### 3.5 Case-Insensitive String Comparison

`equalIgnoreCase()` walks both strings character by character, applying
`tolower()` before comparing, so `"Fiction"` matches `"fiction"`. This is used
for searching books by title/author/category.

---

## 4. How the Program Produces Output (Program Flow)

1. **`main()`** displays the top-level menu in an infinite `do...while` loop
   (exits when the user selects **6. Exit**).
2. Based on the user's numeric choice, control passes to a sub-menu or action:
   - **Book Management** → `bookMenu()` → add/remove/search/update/display books (BST operations)
   - **Student Management** → `studentMenu()` → add/remove/search/display students, view borrowed books & history (Linked List operations)
   - **Issue Book** → `issueBook()` → validates student & book exist, checks availability, decrements `availableCopies`, logs to borrowed list, history, and pushes an "issue" transaction
   - **Return Book** → `returnBook()` → validates the book was actually borrowed, computes the fine via date functions, increments `availableCopies`, logs to history, and pushes a "return" transaction
   - **Undo Last Transaction** → `undoLastTransaction()` → pops the transaction stack and reverses the most recent issue/return
3. Every action function prints formatted confirmation or error messages
   directly via `printf()`, so results are visible immediately after each
   operation — there is no separate "report generation" step; the linked
   list/BST is simply traversed and printed on demand (e.g., `displayAllBooks`,
   `displayAllStudents`).

---

## 5. Summary Table — Where Each DSA Concept Appears

| Concept | Location in Code |
|---|---|
| Singly Linked List | `Student`, `BorrowedBook`, `History` structures |
| Stack (via linked list) | `Transaction` structure + `pushTransaction` / `undoLastTransaction` |
| Binary Search Tree | `Book` structure, `insertBook`, `findBookById`, `deleteBook` |
| Recursion | All BST functions (`insertBook`, `findBookById`, `deleteBook`, `searchByTitle/Author/Category`, `displayAllBooks`) |
| In-order Traversal | `displayAllBooks` (produces sorted-by-ID output) |
| BST Deletion (3 cases) | `deleteBook` — leaf, one child, two children (successor swap) |
| String/Date Utilities | `equalIgnoreCase`, `convertToTime`, `daysBetween`, `calculateFine` |

---

## 6. Possible Viva/Presentation Talking Points

- Why a BST for books but a linked list for students? *(Books are frequently
  searched/sorted by ID; students are typically accessed by direct ID lookup
  and their count is smaller — a BST would be over-engineering there, though
  it would also work.)*
- What happens to BST balance? *(This is a plain/unbalanced BST — worst-case
  O(n) if IDs are inserted in sorted order. A follow-up improvement would be
  an AVL or Red-Black tree for guaranteed O(log n).)*
- Why a stack for undo instead of a list? *(Undo only ever affects the most
  recently performed action — LIFO order — which is exactly what a stack
  models.)*
- How is the fine calculated? *(Difference in calendar days between due date
  and return date, multiplied by a fixed daily rate, using C's `time.h`
  functions.)*
