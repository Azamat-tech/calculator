#include "expressionParser.hpp"

// Trim: https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

void printResult(const variant<int, float, double>& a) {
    switch (a.index()) {
        case 0:
            cout << get<int>(a) << endl;
            break;
        case 1:
            cout << get<float>(a) << endl;
            break;
        case 2:
            cout << get<double>(a) << endl;
            break;
    }    
}

int main(){
    variant<int, float, double> result;
    ExpressionParser evaluator;

    size_t num_expressions;
    cin >> num_expressions;
    cin.ignore(256, '\n');

    string expression;


    while (num_expressions != 0) {
        getline(cin, expression);
        num_expressions--;

        // if line is empty or a new line ignore 
        if (trim(expression).size() == 0) {
            continue;
        }
        
        try {
            evaluator.parseAndEval(expression, result);
            printResult(result);
        } catch (const VariableAssignmentException& e) { 
            cerr << e.what() << endl;
        } catch (const DivisionByZero& e) { 
            cerr << e.what() << endl;
        } catch (const NumberFormatException& e) {
            cerr << e.what() << endl;
        } catch (const VariableNameFormat& e) {
            cerr << e.what() << endl;
        } catch (const LoadingVariableException& e) {
            cerr << e.what() << endl;
        } catch (const VariableNotFoundException& e) {
            cerr << e.what() << endl;
        } catch (const UnknownOperation& e) {
            cerr << e.what() << endl;
        } catch (const IncorrectExpressionFormat& e) {
            cerr << e.what() << endl;
        } catch (const StackEmptyException& e) {
            cerr << e.what() << endl;
        } catch (const MultipleVariableAssignmentException& e) {
            cerr << e.what() << endl;
        } catch (const ModuloException& e) {
            cerr << e.what() << endl;
        }

        evaluator.emptyParsedExpression();
    }
}