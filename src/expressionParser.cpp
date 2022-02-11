#include "../header/expressionParser.hpp"

bool ExpressionParser::isFloat(const string& n) const {
    // checking: if numbers excluding last char are digits and if last char is 'f' and if there is only
    // one dot (to indicate precision) and after the dot there are numbers then it is a float
    return (n.substr(0, n.size()-1)).find_first_not_of("0123456789.") == string::npos && n.back() == 'f'
            && getNumberOfOccurances('.', n) == 1 && n.substr(0, n.size()-1).back() != '.';
}

bool ExpressionParser::isNumber(const string& n) const {
    return n.find_first_not_of("0123456789.") == string::npos || isFloat(n);
}

bool ExpressionParser::isOperator(char ch) { 
    return find(begin(operators), end(operators), ch) != end(operators);
}

bool ExpressionParser::containsAlpha(const string& var_name) const { 
    return regex_match(var_name, regex("^[A-Za-z$]+$"));
}

bool ExpressionParser::isBracket(char ch) const {
    return ch == ')' || ch == '(';
}

int ExpressionParser::getNumberOfOccurances(char ch, const string& entity) const {
    int count = 0;
    for (size_t i = 0; ((i = entity.find(ch, i)) != std::string::npos); i++) {
        count++;
    }
    return count;
}

bool ExpressionParser::checkForMultipleDots(const string& entity) const { 
    
    return entity.find('.') == entity.find_last_of('.') && getNumberOfOccurances('.', entity) < 1;
}

void ExpressionParser::emptyParsedExpression() {
    parsedExpression.clear();
}

void ExpressionParser::processEntity(string& entity) { 
    if (isNumber(entity)) {
        parsedExpression.push_back(entity); // 3+ 2 || (2 - 3) chars that next to each other
        entity.clear();
    } else if (containsAlpha(entity)) { 
        if (entity.front() != '$') {
            throw LoadingVariableException();   // xyz: no $
        }
        parsedExpression.push_back(entity); // 3+ 2 || (2 - 3) chars that next to each other
        entity.clear();
    } else {
        throw VariableNameFormat(); // xy.za
    }
}

void ExpressionParser::parseToVector(const string& expression) { 
    string entity;

    for (size_t i = 0; i < expression.size(); i++) {
        char ch = expression[i];
        if (isspace(ch) || isOperator(ch) || isBracket(ch) || ch == '.') {
            if (!entity.empty()) {
                if (ch == '.') {
                    if (checkForMultipleDots(entity)) { // 2.3 - 4.0
                        entity.push_back(ch);
                        continue;
                    }
                    throw NumberFormatException();  // 2.4.4
                } 
                else if (entity.back() == '.') {
                    throw NumberFormatException();  // 2.4.
                }
                else {
                    processEntity(entity);
                }
                if (ch != ' ') {
                    entity.push_back(ch);
                    parsedExpression.push_back(entity);
                    entity.clear();
                }
            } else {
                if (isOperator(ch) || isBracket(ch)) {
                    entity.push_back(ch);
                    parsedExpression.push_back(entity);
                    entity.clear();
                    continue;
                }
                if (ch == '.') { 
                    throw NumberFormatException(); // .12
                }
            }
        } else {
            entity.push_back(ch);
        }
    }
    if (!entity.empty()) {
        processEntity(entity);
    }
}


bool ExpressionParser::isVarAssignmentCorrect(const string& assignment) const {
    return (find(assignment.begin(), assignment.end(), ' ')) == assignment.end();
}

void ExpressionParser::parseVariableAssignment(string& expression, string& var_name, string& expr) { 
    size_t pos = 0;
    while ((pos = expression.find("=")) != std::string::npos) {
        var_name = expression.substr(0, pos);
        expression.erase(0, pos + 1);
    }
    expr = expression;
    // check if var_name is a proper alphabetic name
    if (getNumberOfOccurances('$', var_name) > 0 || !containsAlpha(var_name)) { // $x= 2 + 2 || x.y= 2 + 2
        throw VariableAssignmentException();
    }
}

// Gets all the positions where implied multiplication will be added
void ExpressionParser::getMinusPositions(vector<size_t>& unaryOpPos) {
    int adder = 0; 
    for (size_t i = 0; i < parsedExpression.size(); i++) { 
        if (parsedExpression[i] == "-") {
            // if at the beginning then unary
            if(i == 0) {    
                unaryOpPos.push_back(i+adder);
                adder++;
            } else {
                // case: 4 - - 2 (my program deals at max with 2 ops next to each other)
                // this case it will take the 2nd op and convert it to unary op
                if (isOperator(parsedExpression[i - 1][0]) || isBracket(parsedExpression[i-1][0])) {
                    unaryOpPos.push_back(i + adder);
                    adder++;
                    continue;
                } 
                unaryOpPos.push_back(i+adder);
                adder+=2;            
            }
        }
        // case: 3 (5 + 2) -> 3 * (5 + 2)
        if (i != 0 && parsedExpression[i][0] == '(' && isNumber(parsedExpression[i-1]) ) {

            unaryOpPos.push_back(i+adder-1);
            adder++;
        }
    }
}

// Takes the index of the variant and based on that returns its string
void ExpressionParser::convertToString(string& toString, const variant<int, float, double>& v) {
    size_t index = v.index();
    switch(index) {
        case 0:
            toString = to_string(get<int>(v));
            break;
        case 1:
            toString = to_string(get<float>(v));
            break;
        case 2:
            toString = to_string(get<double>(v));
            break;
    }
}

// Will convert all variables to their corresponding values from map
void ExpressionParser::changeVariableToValues() {
    for (size_t i = 0; i < parsedExpression.size(); i++) {
        if (containsAlpha(parsedExpression[i])) {
            
            transform(parsedExpression[i].begin(), parsedExpression[i].end(), parsedExpression[i].begin(),
                [](unsigned char c){ return std::tolower(c); });

            auto v = var_storage.find(parsedExpression[i].substr(1));
            if (v == var_storage.end()) {
                throw VariableNotFoundException();
            }
            // replace all variables parsedExpression[i] with string_value
            string string_value;
            convertToString(string_value, v->second);
            replace(parsedExpression.begin(), parsedExpression.end(), parsedExpression[i], string_value);
        }
    }
}

void ExpressionParser::addImpliedValues() { 
    vector<size_t> unaryOpPos;
    getMinusPositions(unaryOpPos);

    size_t increm = 1;
    for(size_t i = 0; i < unaryOpPos.size(); i++) {
        // for every minus ,except the first, do + -1 * "(N"
        if (isNumber(parsedExpression[unaryOpPos[i]]) && parsedExpression[unaryOpPos[i]+1][0] == '(') { 
            parsedExpression.insert(parsedExpression.begin() + unaryOpPos[i] + 1, "*");
        } else {
            parsedExpression[unaryOpPos[i]].push_back('1');
            // case: beginning and (-3) => (-1 * 3) not (+ -1 * 3)
            if (unaryOpPos[i] == 0 || isBracket(parsedExpression[unaryOpPos[i]-1][0])
                || isOperator(parsedExpression[unaryOpPos[i] - 1][0])) {
                increm = 1;
            }
            else {
                // case: every other case where you see `-` becomes => + -1 * ...

                parsedExpression.insert(parsedExpression.begin() + unaryOpPos[i], "+");
                increm = 2;
            }
            parsedExpression.insert(parsedExpression.begin() + unaryOpPos[i] + increm, "*");
        }
    }
    changeVariableToValues();
}

size_t ExpressionParser::getHighestType() {
    size_t highest = 0;

    for (size_t i = 0; i < parsedExpression.size(); i++) {
        if (!isNumber(parsedExpression[i])) { continue; }

        if (CheckInt(parsedExpression[i])) {
            continue;
        } else if (CheckFloat(parsedExpression[i])){
            if (highest < 1) {
                highest = 1;
            }
        } else {
            highest = 2;
            return highest;
        }
    }
    return highest;
}

bool ExpressionParser::CheckInt(const string& v) const {
    try {
        size_t p;
        stoi(v, &p);
        // Compare the size of the converted to the whole string
        if (p == v.size()) {
            return true;
        }
        return false;
    } catch (out_of_range& e) {
        return false;
    }
    catch(std::invalid_argument& e){
        // if no conversion could be performed
        return false;
    }
}

bool ExpressionParser::CheckFloat(const string& v) const {
    try {
        if (!isFloat(v)) {
            return false;
        }
        stof(v);
        return true;
    } catch (out_of_range& e) {
        return false;
    }
    catch(std::invalid_argument& e){
        // if no conversion could be performed
        return false;
    }
}

void ExpressionParser::evaluate(variant<int, float, double>& output) { 
    size_t expr_type = getHighestType();
    
    switch(expr_type) {
        case 0: {
            ExpressionEvaluator<int> evaluator;
            output = evaluator.process(parsedExpression);
            break;
        }
        case 1: {
            ExpressionEvaluator<float> evaluator;
            output = evaluator.process(parsedExpression);
            break;
        }
        case 2: {
            ExpressionEvaluator<double> evaluator;
            output = evaluator.process(parsedExpression);
            break;
        }    
    }
}

void ExpressionParser::storeVariable(string& var_name, const variant<int, float, double>& value) {
    transform(var_name.begin(), var_name.end(), var_name.begin(),
                [](unsigned char c){ return std::tolower(c); });

    auto r = var_storage.find(var_name);
    // the variable already been assigned a value
    if (r != var_storage.end()) {
        throw MultipleVariableAssignmentException();
    } 
    // not assigned yet
    else {
        var_storage.insert(pair<string, variant<int, float, double>>(var_name, value));
    }
}

void ExpressionParser::parseAndEval(string& expression, variant<int, float, double>& output) {
    if (expression.find("=") != string::npos) {
        string var_name, expr;

        parseVariableAssignment(expression, var_name, expr);

        if (!isVarAssignmentCorrect(var_name)) {
            throw VariableAssignmentException();
        }

        parseToVector(expr);
        addImpliedValues();

        evaluate(output);

        storeVariable(var_name, output);
    } else {
        parseToVector(expression);
        addImpliedValues();

        evaluate(output);
    }
}

void ExpressionParser::print() const {
    cout << "Expression: ";
    for (size_t i = 0; i < parsedExpression.size();i++) {
        cout << parsedExpression[i] << " ";
    }
    cout << endl;
} 