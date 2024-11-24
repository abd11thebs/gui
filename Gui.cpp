#include<iostream>
#include<fstream>
#include<ctime>
#include<sstream>
#include<regex>
#include<vector>
#include<map>
#include<iomanip>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
using namespace std;

struct Transaction
{
    double amount;
    string category;
    string date; //year-month-day//
};

string todaysdate()
{
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    ostringstream oss;
    oss << put_time(now, "%Y-%m-%d");
    return oss.str();
}

bool datevalid(string date)
{
    int year, month, day;
    regex datepattern("\\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])");
    if (!regex_match(date,datepattern))
    {
        return false;
    }
    
    year = stoi(date.substr(0,4));
    month = stoi(date.substr(5,2));
    day = stoi(date.substr(8,2));

    int daysofmonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        daysofmonth[1] = 29;
    }

    if ((day< 1 || day > daysofmonth[month-1]))
    {
        return false;
    }
    return true;
}

void readfromfile(vector<Transaction> &transaction_data, string &data_file)
{
    ifstream file(data_file);

    if (!file.good()){
        ofstream file(data_file);
        cout<<"File didn't exist. New file created"<<endl;
        return;
    }

    string line;
    string letter;
    int firstspace;
    int count;

    while(getline(file, line)){
        Transaction t;
        count = 0;
        for (int i = 0; i < line.length(); i++)
        {
            letter = line.substr(i,1);
            if (letter == " "){
                count++;
                if (count == 1){
                    t.amount = stod(line.substr(0,i));
                    firstspace = i;
                }
                else if (count == 2){
                    t.category = line.substr(firstspace+1,i-firstspace-1);
                    t.date = line.substr(i+1);
                    break;
                }
            }
        }
        transaction_data.push_back(t);
    }
    file.close();
}

void savetofile(const vector<Transaction> &transaction_data, string &data_file)
{
    ofstream file(data_file);

    if (!file){
        cout<<"Error opening file"<<endl;
        return;
    }

    for (int i =0; i<transaction_data.size();i++)
    {
        file << transaction_data[i].amount <<" "<< transaction_data[i].category<< " "<<transaction_data[i].date<<endl;
    }
    file.close();
    cout<<"Data saved to "<< data_file<<endl;
}

void savetofile_limits(const map<string,double> &category_limits, string &data_file)
{
    ofstream file(data_file);

    if (!file){
        cout<<"Error opening 'category-limits' file"<<endl;
        return;
    }

    for (map<string, double>::const_iterator it = category_limits.begin(); it != category_limits.end(); ++it)
    {
        file << it->first<<" "<< it->second <<endl;
    }
    file.close();
    
}

void readfromfile_limits(map<string,double> &category_limits, string &data_file)
{
    ifstream file(data_file);

    if (!file.good()){
        ofstream file(data_file);
        return;
    }

    string line;
    string letter;
    string category;
    double limit;

    while(std::getline(file, line)){
        for (int i = 0; i < line.length(); i++)
        {
            letter = line.substr(i,1);
            if (letter == " "){
                category = line.substr(0,i);
                limit = stod(line.substr(i+1));
                break;
                }
        }
        category_limits[category] = limit;
    }
        
    file.close();
}

void addtransaction(vector<Transaction>& transaction_data)
{
    string category, date;
    double amount;
    bool flag = false;
    bool flag2 = false;
    bool flag3 = false;
    char inputchar;
    cin.ignore();

    while(!flag3){
        cout<<"Enter 'e' for expense or 'i' for income: "<<endl;
        cin >> inputchar;
        if (inputchar == 'e' || inputchar == 'i'){
            flag3 = true;
        }    
    }

    while (!flag2) {
        cout << "Enter amount(PKR): "<<endl;
        cin >> amount;
        cin.ignore();

        if (cin.fail()) {
            cout << "Invalid input! Please enter a valid number for the amount." << endl;
            cin.clear(); //resets error state//
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //discards data in the buffer until a new line//
            } 
        else {
            flag2 = true;
        }
    }

    if(inputchar == 'e'){
        amount = -amount;
    }

    cout<<"Enter category: "<<endl;
    getline(cin,category);

    while (category.empty()) {
        cout << "Category cannot be empty. Please enter again: " << endl;
        getline(cin, category);
    }

    cout<<"Enter date in the format YYYY-MM-DD or type 'today' to get today's date: "<<endl;
    getline(cin,date);
    
    
    while (flag == false){
        if (date == "today"){
            date = todaysdate();
            flag = true;
            break;
        }
        else if(datevalid(date) == true){
            flag = true;
            break;
        }
        else{
            cout<<"Error! Enter date in the correct format (YYYY-MM-DD) or type 'today': "<<endl;
            getline(cin,date);
            
        }
    }
    Transaction t1 = {amount,category,date};
    transaction_data.push_back(t1);
}

void monthlysummary_category(const vector<Transaction> &transaction_data,map<string, double> &category_limits)
{
    bool flag = false;
    string user_month;
    string datecheck;
    string date;
    double total_income = 0;
    double total_expense = 0;
    cin.ignore();

    cout<<"Enter month in the format YYYY-MM or type 'today' for the current month: "<<endl;
    getline(cin,date);
    datecheck = date + "-10";

    while (flag == false){
        if (date == "today"){
            date = todaysdate().substr(0,7);
            flag = true;
            break;
        }
        else if(datevalid(datecheck) == true){
            flag = true;
            break;
        }
        else{
            cout<<"Error! Enter month in the correct format (YYYY-MM) or type 'today': "<<endl;
            getline(cin,date);
            datecheck = date + "-10";
        }
    }

    map<string, double> category_summary;
    double net;

    for(vector<Transaction>::const_iterator it = transaction_data.begin(); it != transaction_data.end(); ++it){
        if (it->date.substr(0,7) == date){
            category_summary[it->category] += it->amount;
            net += it->amount;
            if (it->amount > 0){
                total_income += it->amount;
            }
            else if(it->amount < 0){
                total_expense += it->amount;
            }
        }
    }

    if(category_summary.empty()){
        cout<<"No transactions found"<<endl;
        return;
    }

    cout<<"Summary for "<< date<<endl;
    cout << "--------------------------------------"<<endl;
    cout<< setw(15) << "Category" << setw(15) << "Amount" << endl;

    for (map<string, double>::iterator it = category_summary.begin(); it != category_summary.end(); ++it) {
        cout << setw(15) <<it->first <<setw(15) << it->second;

        map<string, double>::const_iterator limit_it = category_limits.find(it->first);
        if (limit_it != category_limits.end()) {
            if (-(it->second) > limit_it->second) { 
                cout << setw(15)<<" (Limit Exceeded) Limit: "<<-(limit_it->second);
            }
        }
        cout<<endl;
    }

    cout << "--------------------------------------"<<endl;
    cout<<"Total income: "<< total_income<<endl;
    cout<<"Total expense: "<< (-total_expense)<<endl;
    cout << "Net Balance: " << net << endl;
}

void applylimtis(map<string, double> &category_limits){

    double limit;
    string category;
    bool flag = false;
    cin.ignore();

    cout<<"Enter category to apply budget limits on: "<<endl;
    getline(cin,category);

    while (category.empty()) {
        cout << "Category cannot be empty. Please enter again: " << endl;
        getline(cin, category);
    }

    while (!flag) {
        cout << "Enter budget limit(PKR) or enter '0' to reset a budget limit: ";
        cin >> limit;
        cin.ignore();

        if (cin.fail()) {
            cout << "Invalid input! Please enter a valid number for the limit." << endl;
            cin.clear(); //resets error state//
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //discards data in the buffer until a new line//
            } 
        else {
            flag = true;
        }
    }

    if(limit == 0){
        limit = 1.7976931348623157E+308; //resetting a limit//
    }

    category_limits[category] = limit;
    if(limit == 1.7976931348623157E+308){
        cout<<"Budget limit reset for "<<category<<endl;
    }
    else{
        cout<<"Budget limit for "<<category<<" "<<"set to "<<limit<<endl;
    }

}

struct Transaction {
    double amount;
    string category;
    string date; // year-month-day
};

// Add your original functions here, e.g., todaysdate(), datevalid(), readfromfile(), etc.

// GUI Menu
void renderGUI(vector<Transaction> &transaction_data, map<string, double> &category_limits, string &transactions_file, string &limits_file) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Personal Budget Manager");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font!" << endl;
        return;
    }

    // Menu options
    sf::Text title("Personal Budget Manager", font, 30);
    title.setPosition(200, 50);
    title.setFillColor(sf::Color::White);

    sf::Text option1("1. Add Transaction", font, 20);
    option1.setPosition(100, 150);
    option1.setFillColor(sf::Color::White);

    sf::Text option2("2. View Monthly Summary (Category-wise)", font, 20);
    option2.setPosition(100, 200);
    option2.setFillColor(sf::Color::White);

    sf::Text option3("3. Set Monthly Budget Limits (Category-wise)", font, 20);
    option3.setPosition(100, 250);
    option3.setFillColor(sf::Color::White);

    sf::Text option4("4. Exit", font, 20);
    option4.setPosition(100, 300);
    option4.setFillColor(sf::Color::White);

    // Event loop for GUI interactions
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Check button clicks based on positions
                    if (mousePos.x >= 100 && mousePos.x <= 500) {
                        if (mousePos.y >= 150 && mousePos.y <= 170) {
                            // Add Transaction
                            addtransaction(transaction_data);
                        } else if (mousePos.y >= 200 && mousePos.y <= 220) {
                            // View Monthly Summary
                            monthlysummary_category(transaction_data, category_limits);
                        } else if (mousePos.y >= 250 && mousePos.y <= 270) {
                            // Set Budget Limits
                            applylimtis(category_limits);
                        } else if (mousePos.y >= 300 && mousePos.y <= 320) {
                            // Exit
                            window.close();
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(title);
        window.draw(option1);
        window.draw(option2);
        window.draw(option3);
        window.draw(option4);
        window.display();
    }

    // Save data before exiting
    savetofile(transaction_data, transactions_file);
    savetofile_limits(category_limits, limits_file);
}


int main() {
    string transactions_file = "transactions.txt";
    string limits_file = "limits.txt";
    vector<Transaction> transaction_data;
    map<string, double> category_limits;

    // Original logic for reading files
    readfromfile(transaction_data, transactions_file);
    readfromfile_limits(category_limits, limits_file);

    // Launch GUI
    renderGUI(transaction_data, category_limits, transactions_file, limits_file);

    return 0;
}
