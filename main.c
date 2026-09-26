#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* =========================================================
   STRUCTURES
   ========================================================= */

/* ---------- Borrowed Book Linked List ---------- */

struct BorrowedBook {
    int bookId;
    int dueDate;
    struct BorrowedBook *next;
};

/* ---------- Borrowing History Linked List ---------- */

struct History {
    int bookId;
    char action[20];
    int date;
    float fine;
    struct History *next;
};

/* ---------- Student Linked List ---------- */

struct Student {
    int id;
    char name[50];
    char department[30];

    struct BorrowedBook *borrowedBooks;
    struct History *history;

    struct Student *next;
};

/* ---------- Book BST ---------- */

struct Book {
    int id;
    char title[100];
    char author[50];
    char category[30];

    int totalCopies;
    int availableCopies;

    struct Book *left;
    struct Book *right;
};

/* ---------- Undo Stack ---------- */

struct Transaction {
    char type[20];
    int bookId;
    int studentId;

    int dueDate;
    int transactionDate;
    float fine;

    struct Transaction *next;
};


/* =========================================================
   GLOBAL VARIABLES
   ========================================================= */

struct Student *head = NULL;
struct Book *root = NULL;
struct Transaction *top = NULL;


/* =========================================================
   UTILITY FUNCTIONS
   ========================================================= */

void readString(char str[], int size)
{
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}

int equalIgnoreCase(char a[], char b[])
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {

        if (tolower((unsigned char)a[i]) !=
            tolower((unsigned char)b[i])) {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}


/* =========================================================
   DATE FUNCTIONS
   ========================================================= */

time_t convertToTime(int date)
{
    int year = date / 10000;
    int month = (date / 100) % 100;
    int day = date % 100;

    struct tm t;

    memset(&t, 0, sizeof(t));

    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;

    return mktime(&t);
}

int daysBetween(int date1, int date2)
{
    time_t t1 = convertToTime(date1);
    time_t t2 = convertToTime(date2);

    double difference = difftime(t2, t1);

    return (int)(difference / (60 * 60 * 24));
}

float calculateFine(int dueDate, int returnDate)
{
    int lateDays;

    lateDays = daysBetween(dueDate, returnDate);

    if (lateDays <= 0)
        return 0;

    return lateDays * 5.0;
}


/* =========================================================
   STUDENT MANAGEMENT - LINKED LIST
   ========================================================= */

struct Student *findStudent(int id)
{
    struct Student *temp = head;

    while (temp != NULL) {

        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}


/* Requirement 10 - Add Student */

void addStudent()
{
    struct Student *newStudent;
    struct Student *temp;

    newStudent =
        (struct Student *)malloc(sizeof(struct Student));

    if (newStudent == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("\nEnter student ID: ");
    scanf("%d", &newStudent->id);
    getchar();

    if (findStudent(newStudent->id) != NULL) {
        printf("Student ID already exists!\n");
        free(newStudent);
        return;
    }

    printf("Enter name: ");
    readString(newStudent->name, 50);

    printf("Enter department: ");
    readString(newStudent->department, 30);

    newStudent->borrowedBooks = NULL;
    newStudent->history = NULL;
    newStudent->next = NULL;

    if (head == NULL) {
        head = newStudent;
    }
    else {
        temp = head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newStudent;
    }

    printf("Student added successfully!\n");
}


/* Requirement 11 - Remove Student */

void removeStudent()
{
    int id;

    struct Student *temp;
    struct Student *prev;

    printf("\nEnter student ID to remove: ");
    scanf("%d", &id);

    temp = head;
    prev = NULL;

    while (temp != NULL && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Student not found!\n");
        return;
    }

    if (temp->borrowedBooks != NULL) {
        printf("Cannot remove student!\n");
        printf("Student currently has borrowed books.\n");
        return;
    }

    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;

    free(temp);

    printf("Student removed successfully!\n");
}


/* Requirement 12 - Search Student */

void searchById()
{
    int id;

    struct Student *student;

    printf("\nEnter student ID to search: ");
    scanf("%d", &id);

    student = findStudent(id);

    if (student == NULL) {
        printf("Student not found!\n");
        return;
    }

    printf("\nStudent found!\n");
    printf("ID         : %d\n", student->id);
    printf("Name       : %s\n", student->name);
    printf("Department : %s\n", student->department);
}


/* Requirement 15 - Display All Students */

void displayAllStudents()
{
    struct Student *temp = head;

    if (temp == NULL) {
        printf("\nNo students registered!\n");
        return;
    }

    printf("\n========== ALL STUDENTS ==========\n");

    while (temp != NULL) {

        printf("ID         : %d\n", temp->id);
        printf("Name       : %s\n", temp->name);
        printf("Department : %s\n", temp->department);

        printf("----------------------------------\n");

        temp = temp->next;
    }
}


/* =========================================================
   BOOK MANAGEMENT - BST
   ========================================================= */

/* Find book by ID - BST SEARCH */

struct Book *findBookById(struct Book *root, int id)
{
    if (root == NULL)
        return NULL;

    if (root->id == id)
        return root;

    if (id < root->id)
        return findBookById(root->left, id);

    return findBookById(root->right, id);
}


/* Create Book */

struct Book *createBook()
{
    struct Book *newBook;

    newBook =
        (struct Book *)malloc(sizeof(struct Book));

    if (newBook == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    printf("\nEnter Book ID: ");
    scanf("%d", &newBook->id);
    getchar();

    printf("Enter title: ");
    readString(newBook->title, 100);

    printf("Enter author: ");
    readString(newBook->author, 50);

    printf("Enter category: ");
    readString(newBook->category, 30);

    printf("Enter total copies: ");
    scanf("%d", &newBook->totalCopies);

    printf("Enter available copies: ");
    scanf("%d", &newBook->availableCopies);

    if (newBook->availableCopies > newBook->totalCopies)
        newBook->availableCopies = newBook->totalCopies;

    if (newBook->availableCopies < 0)
        newBook->availableCopies = 0;

    newBook->left = NULL;
    newBook->right = NULL;

    return newBook;
}


/* Insert Book into BST */

struct Book *insertBook(struct Book *root,
                        struct Book *newBook)
{
    if (root == NULL)
        return newBook;

    if (newBook->id < root->id) {

        root->left =
            insertBook(root->left, newBook);
    }
    else if (newBook->id > root->id) {

        root->right =
            insertBook(root->right, newBook);
    }
    else {

        printf("Book ID already exists!\n");
        free(newBook);
    }

    return root;
}


/* Requirement 1 - Add Book */

void addBook()
{
    struct Book *newBook;

    newBook = createBook();

    if (newBook == NULL)
        return;

    if (findBookById(root, newBook->id) != NULL) {

        printf("Book ID already exists!\n");
        free(newBook);
        return;
    }

    root = insertBook(root, newBook);

    printf("Book added successfully!\n");
}


/* Display Book Information */

void displayBook(struct Book *book)
{
    printf("\n--------------------------------------\n");

    printf("Book ID          : %d\n", book->id);
    printf("Title            : %s\n", book->title);
    printf("Author           : %s\n", book->author);
    printf("Category         : %s\n", book->category);
    printf("Total Copies     : %d\n", book->totalCopies);
    printf("Available Copies : %d\n", book->availableCopies);

    if (book->availableCopies > 0)
        printf("Status           : Available\n");
    else
        printf("Status           : Not Available\n");

    printf("--------------------------------------\n");
}


/* Requirement 3 - Search Book by ID */

void searchBookById()
{
    int id;

    struct Book *book;

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    book = findBookById(root, id);

    if (book == NULL) {
        printf("Book not found!\n");
        return;
    }

    displayBook(book);
}


/* Search Book by Title */

void searchByTitle(struct Book *root,
                   char title[],
                   int *found)
{
    if (root == NULL)
        return;

    searchByTitle(root->left, title, found);

    if (equalIgnoreCase(root->title, title)) {
        displayBook(root);
        *found = 1;
    }

    searchByTitle(root->right, title, found);
}


/* Requirement 4 */

void searchBookByTitle()
{
    char title[100];
    int found = 0;

    getchar();

    printf("\nEnter book title: ");
    readString(title, 100);

    searchByTitle(root, title, &found);

    if (!found)
        printf("No book found with that title.\n");
}


/* Search Book by Author */

void searchByAuthor(struct Book *root,
                    char author[],
                    int *found)
{
    if (root == NULL)
        return;

    searchByAuthor(root->left, author, found);

    if (equalIgnoreCase(root->author, author)) {
        displayBook(root);
        *found = 1;
    }

    searchByAuthor(root->right, author, found);
}


/* Requirement 5 */

void searchBookByAuthor()
{
    char author[50];
    int found = 0;

    getchar();

    printf("\nEnter author name: ");
    readString(author, 50);

    searchByAuthor(root, author, &found);

    if (!found)
        printf("No books found by that author.\n");
}


/* Search Book by Category */

void searchByCategory(struct Book *root,
                      char category[],
                      int *found)
{
    if (root == NULL)
        return;

    searchByCategory(root->left, category, found);

    if (equalIgnoreCase(root->category, category)) {
        displayBook(root);
        *found = 1;
    }

    searchByCategory(root->right, category, found);
}


/* Requirement 6 */

void searchBookByCategory()
{
    char category[30];
    int found = 0;

    getchar();

    printf("\nEnter category: ");
    readString(category, 30);

    searchByCategory(root, category, &found);

    if (!found)
        printf("No books found in that category.\n");
}


/* Requirement 8
   In-order traversal
   Displays books in ascending Book ID order
*/

void displayAllBooks(struct Book *root)
{
    if (root == NULL)
        return;

    displayAllBooks(root->left);

    displayBook(root);

    displayAllBooks(root->right);
}


/* Find Minimum Node */

struct Book *findMin(struct Book *root)
{
    struct Book *temp = root;

    while (temp != NULL && temp->left != NULL)
        temp = temp->left;

    return temp;
}


/* Requirement 2 - Delete Book */

struct Book *deleteBook(struct Book *root,
                        int id,
                        int *deleted)
{
    struct Book *temp;

    if (root == NULL)
        return NULL;

    if (id < root->id) {

        root->left =
            deleteBook(root->left,
                       id,
                       deleted);
    }

    else if (id > root->id) {

        root->right =
            deleteBook(root->right,
                       id,
                       deleted);
    }

    else {

        *deleted = 1;

        /* No child */

        if (root->left == NULL &&
            root->right == NULL) {

            free(root);
            return NULL;
        }

        /* Only right child */

        else if (root->left == NULL) {

            temp = root->right;

            free(root);

            return temp;
        }

        /* Only left child */

        else if (root->right == NULL) {

            temp = root->left;

            free(root);

            return temp;
        }

        /* Two children */

        else {

            temp = findMin(root->right);

            root->id = temp->id;

            strcpy(root->title,
                   temp->title);

            strcpy(root->author,
                   temp->author);

            strcpy(root->category,
                   temp->category);

            root->totalCopies =
                temp->totalCopies;

            root->availableCopies =
                temp->availableCopies;

            root->right =
                deleteBook(root->right,
                           temp->id,
                           deleted);
        }
    }

    return root;
}


void removeBook()
{
    int id;
    int deleted = 0;

    printf("\nEnter Book ID to remove: ");
    scanf("%d", &id);

    if (findBookById(root, id) == NULL) {

        printf("Book not found!\n");
        return;
    }

    root = deleteBook(root,
                      id,
                      &deleted);

    if (deleted)
        printf("Book removed successfully!\n");
}


/* Requirement 9 - Update Book */

void updateBook()
{
    int id;
    int choice;
    int copies;

    struct Book *book;

    printf("\nEnter Book ID to update: ");
    scanf("%d", &id);

    book = findBookById(root, id);

    if (book == NULL) {

        printf("Book not found!\n");
        return;
    }

    printf("\n1. Add copies\n");
    printf("2. Change category\n");
    printf("3. Change author\n");
    printf("4. Change title\n");

    printf("Enter choice: ");
    scanf("%d", &choice);

    getchar();

    switch (choice) {

        case 1:

            printf("Enter number of new copies: ");
            scanf("%d", &copies);

            if (copies > 0) {

                book->totalCopies += copies;
                book->availableCopies += copies;

                printf("Copies added successfully!\n");
            }

            break;

        case 2:

            printf("Enter new category: ");
            readString(book->category, 30);

            printf("Category updated!\n");

            break;

        case 3:

            printf("Enter new author: ");
            readString(book->author, 50);

            printf("Author updated!\n");

            break;

        case 4:

            printf("Enter new title: ");
            readString(book->title, 100);

            printf("Title updated!\n");

            break;

        default:

            printf("Invalid choice!\n");
    }
}


/* =========================================================
   BORROWED BOOK MANAGEMENT
   ========================================================= */

void addBorrowedBook(struct Student *student,
                     int bookId,
                     int dueDate)
{
    struct BorrowedBook *newBook;
    struct BorrowedBook *temp;

    newBook =
        (struct BorrowedBook *)
        malloc(sizeof(struct BorrowedBook));

    if (newBook == NULL) {

        printf("Memory allocation failed!\n");
        return;
    }

    newBook->bookId = bookId;
    newBook->dueDate = dueDate;
    newBook->next = NULL;

    if (student->borrowedBooks == NULL) {

        student->borrowedBooks = newBook;
    }
    else {

        temp = student->borrowedBooks;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newBook;
    }
}


int removeBorrowedBook(struct Student *student,
                        int bookId,
                        int *dueDate)
{
    struct BorrowedBook *temp;
    struct BorrowedBook *prev;

    temp = student->borrowedBooks;
    prev = NULL;

    while (temp != NULL &&
           temp->bookId != bookId) {

        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return 0;

    if (dueDate != NULL)
        *dueDate = temp->dueDate;

    if (prev == NULL)
        student->borrowedBooks = temp->next;
    else
        prev->next = temp->next;

    free(temp);

    return 1;
}


int hasBorrowedBook(struct Student *student,
                    int bookId)
{
    struct BorrowedBook *temp;

    temp = student->borrowedBooks;

    while (temp != NULL) {

        if (temp->bookId == bookId)
            return 1;

        temp = temp->next;
    }

    return 0;
}


/* =========================================================
   BORROWING HISTORY
   ========================================================= */

void addHistory(struct Student *student,
                int bookId,
                char action[],
                int date,
                float fine)
{
    struct History *newHistory;
    struct History *temp;

    newHistory =
        (struct History *)
        malloc(sizeof(struct History));

    if (newHistory == NULL) {

        printf("Memory allocation failed!\n");
        return;
    }

    newHistory->bookId = bookId;

    strcpy(newHistory->action,
           action);

    newHistory->date = date;
    newHistory->fine = fine;
    newHistory->next = NULL;

    if (student->history == NULL) {

        student->history = newHistory;
    }
    else {

        temp = student->history;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newHistory;
    }
}


/* Requirement 13 */

void displayBorrowedBooks()
{
    int studentId;

    struct Student *student;
    struct BorrowedBook *temp;

    printf("\nEnter student ID: ");
    scanf("%d", &studentId);

    student = findStudent(studentId);

    if (student == NULL) {

        printf("Student not found!\n");
        return;
    }

    temp = student->borrowedBooks;

    if (temp == NULL) {

        printf("Student has no borrowed books.\n");
        return;
    }

    printf("\n========== BORROWED BOOKS ==========\n");

    while (temp != NULL) {

        struct Book *book =
            findBookById(root,
                         temp->bookId);

        printf("Book ID  : %d\n",
               temp->bookId);

        if (book != NULL)
            printf("Title    : %s\n",
                   book->title);

        printf("Due Date : %d\n",
               temp->dueDate);

        printf("------------------------------------\n");

        temp = temp->next;
    }
}


/* Requirement 14 */

void displayHistory()
{
    int studentId;

    struct Student *student;
    struct History *temp;

    printf("\nEnter student ID: ");
    scanf("%d", &studentId);

    student = findStudent(studentId);

    if (student == NULL) {

        printf("Student not found!\n");
        return;
    }

    temp = student->history;

    if (temp == NULL) {

        printf("No borrowing history found.\n");
        return;
    }

    printf("\n========== BORROWING HISTORY ==========\n");

    while (temp != NULL) {

        struct Book *book =
            findBookById(root,
                         temp->bookId);

        printf("Book ID : %d\n",
               temp->bookId);

        if (book != NULL)
            printf("Title   : %s\n",
                   book->title);

        printf("Action  : %s\n",
               temp->action);

        printf("Date    : %d\n",
               temp->date);

        printf("Fine    : Rs. %.2f\n",
               temp->fine);

        printf("---------------------------------------\n");

        temp = temp->next;
    }
}


/* =========================================================
   UNDO STACK
   ========================================================= */

void pushTransaction(char type[],
                     int bookId,
                     int studentId,
                     int dueDate,
                     int transactionDate,
                     float fine)
{
    struct Transaction *newTransaction;

    newTransaction =
        (struct Transaction *)
        malloc(sizeof(struct Transaction));

    if (newTransaction == NULL) {

        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(newTransaction->type,
           type);

    newTransaction->bookId = bookId;
    newTransaction->studentId = studentId;

    newTransaction->dueDate = dueDate;
    newTransaction->transactionDate =
        transactionDate;

    newTransaction->fine = fine;

    newTransaction->next = top;

    top = newTransaction;
}


/* =========================================================
   ISSUE BOOK
   ========================================================= */

/* Requirement 16 */

void issueBook()
{
    int studentId;
    int bookId;
    int dueDate;

    struct Student *student;
    struct Book *book;

    printf("\nEnter Student ID: ");
    scanf("%d", &studentId);

    student = findStudent(studentId);

    if (student == NULL) {

        printf("Student not found!\n");
        return;
    }

    printf("Enter Book ID: ");
    scanf("%d", &bookId);

    book = findBookById(root, bookId);

    if (book == NULL) {

        printf("Book not found!\n");
        return;
    }

    if (hasBorrowedBook(student, bookId)) {

        printf("This student already has this book.\n");
        return;
    }

    if (book->availableCopies > 0) {

        printf("Enter due date (YYYYMMDD): ");
        scanf("%d", &dueDate);

        book->availableCopies--;

        addBorrowedBook(student,
                        bookId,
                        dueDate);

        addHistory(student,
                   bookId,
                   "Issued",
                   dueDate,
                   0);

        pushTransaction("issue",
                        bookId,
                        studentId,
                        dueDate,
                        dueDate,
                        0);

        printf("\nBook issued successfully!\n");
        printf("Book     : %s\n",
               book->title);
        printf("Due Date : %d\n",
               dueDate);
    }
    else {

        printf("\nBook is not available.\n");
    }
}


/* =========================================================
   RETURN BOOK
   ========================================================= */

/* Requirement 17 and 20 */

void returnBook()
{
    int studentId;
    int bookId;
    int returnDate;
    int dueDate;

    float fine;

    struct Student *student;
    struct Book *book;

    printf("\nEnter Student ID: ");
    scanf("%d", &studentId);

    student = findStudent(studentId);

    if (student == NULL) {

        printf("Student not found!\n");
        return;
    }

    printf("Enter Book ID: ");
    scanf("%d", &bookId);

    book = findBookById(root, bookId);

    if (book == NULL) {

        printf("Book not found!\n");
        return;
    }

    if (!hasBorrowedBook(student, bookId)) {

        printf("This student has not borrowed this book.\n");
        return;
    }

    printf("Enter return date (YYYYMMDD): ");
    scanf("%d", &returnDate);

    if (!removeBorrowedBook(student,
                            bookId,
                            &dueDate)) {

        printf("Error removing borrowed book.\n");
        return;
    }

    fine = calculateFine(dueDate,
                         returnDate);

    book->availableCopies++;

    addHistory(student,
               bookId,
               "Returned",
               returnDate,
               fine);

    pushTransaction("return",
                    bookId,
                    studentId,
                    dueDate,
                    returnDate,
                    fine);

    printf("\nBook returned successfully!\n");

    if (fine > 0)
        printf("Fine: Rs. %.2f\n", fine);
    else
        printf("No fine.\n");
}


/* =========================================================
   UNDO LAST TRANSACTION
   ========================================================= */

/* Requirement 22 */

void undoLastTransaction()
{
    struct Transaction *transaction;

    struct Student *student;
    struct Book *book;

    if (top == NULL) {

        printf("\nNo transaction to undo.\n");
        return;
    }

    transaction = top;

    book =
        findBookById(root,
                     transaction->bookId);

    student =
        findStudent(transaction->studentId);

    if (book == NULL || student == NULL) {

        printf("Cannot undo transaction.\n");
        return;
    }


    /* Undo ISSUE */

    if (strcmp(transaction->type,
               "issue") == 0) {

        removeBorrowedBook(student,
                           transaction->bookId,
                           NULL);

        book->availableCopies++;

        printf("\nLast ISSUE transaction undone.\n");
        printf("Book returned to available copies.\n");
    }


    /* Undo RETURN */

    else if (strcmp(transaction->type,
                    "return") == 0) {

        if (book->availableCopies > 0) {

            book->availableCopies--;

            addBorrowedBook(student,
                            transaction->bookId,
                            transaction->dueDate);

            printf("\nLast RETURN transaction undone.\n");
            printf("Book re-issued to the student.\n");
        }
        else {

            printf("\nCannot undo return because no available copy exists.\n");
            return;
        }
    }

    top = top->next;

    free(transaction);
}


/* =========================================================
   BOOK MENU
   ========================================================= */

void bookMenu()
{
    int choice;

    do {

        printf("\n\n========== BOOK MANAGEMENT ==========\n");

        printf("1. Add Book\n");
        printf("2. Remove Book\n");
        printf("3. Search Book by ID\n");
        printf("4. Search Book by Title\n");
        printf("5. Search Book by Author\n");
        printf("6. Search Book by Category\n");
        printf("7. Display All Books\n");
        printf("8. Update Book\n");
        printf("9. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                addBook();
                break;

            case 2:
                removeBook();
                break;

            case 3:
                searchBookById();
                break;

            case 4:
                searchBookByTitle();
                break;

            case 5:
                searchBookByAuthor();
                break;

            case 6:
                searchBookByCategory();
                break;

            case 7:

                if (root == NULL)
                    printf("\nNo books available!\n");
                else {
                    printf("\n========== ALL BOOKS ==========\n");
                    displayAllBooks(root);
                }

                break;

            case 8:
                updateBook();
                break;

            case 9:
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 9);
}


/* =========================================================
   STUDENT MENU
   ========================================================= */

void studentMenu()
{
    int choice;

    do {

        printf("\n\n========== STUDENT MANAGEMENT ==========\n");

        printf("1. Add Student\n");
        printf("2. Remove Student\n");
        printf("3. Search Student by ID\n");
        printf("4. Display All Students\n");
        printf("5. Display Borrowed Books\n");
        printf("6. Display Borrowing History\n");
        printf("7. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                addStudent();
                break;

            case 2:
                removeStudent();
                break;

            case 3:
                searchById();
                break;

            case 4:
                displayAllStudents();
                break;

            case 5:
                displayBorrowedBooks();
                break;

            case 6:
                displayHistory();
                break;

            case 7:
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 7);
}


/* =========================================================
   MAIN MENU
   ========================================================= */

int main()
{
    int choice;

    do {

        printf("\n\n");
        printf("==============================================\n");
        printf("        LIBRARY MANAGEMENT SYSTEM\n");
        printf("==============================================\n");

        printf("1. Book Management\n");
        printf("2. Student Management\n");
        printf("3. Issue Book\n");
        printf("4. Return Book\n");
        printf("5. Undo Last Transaction\n");
        printf("6. Exit\n");

        printf("==============================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                bookMenu();
                break;

            case 2:
                studentMenu();
                break;

            case 3:
                issueBook();
                break;

            case 4:
                returnBook();
                break;

            case 5:
                undoLastTransaction();
                break;

            case 6:
                printf("\nExiting Library Management System...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 6);

    return 0;
}
