
#include "expressionEvaluator.hpp"

template <class T>
void ExpressionEvaluator<T>::performOperation() {
    if (stackOperands.size() < 2 || stackOperators.empty()) {
        throw StackEmptyException();
    }

    ValueExpression<T> a = stackOperands.top();
    stackOperands.pop();

    ValueExpression<T> b = stackOperands.top();
    stackOperands.pop();

    unique_ptr<BinaryOperator<T>> op = move(stackOperators.top());
    stackOperators.pop();

    stackOperands.push(ValueExpression (op.get()->evaluate(b.getValue(), a.getValue())));
}

template <class T>
const T ExpressionEvaluator<T>::getStringFromType(const string &i) const {
    if (is_same_v<T, int>){
        return stoi(i);
    }
    else if (is_same_v<T, float>){
        return stof(i);
    }
    else {
        return stod(i);
    }
    throw exception();  
}

template <class T>
const T ExpressionEvaluator<T>::process(const vector<string>& expression) {
    for (const string& value: expression) {
        if (value.size() == 1 && ExpressionParser::isOperator(value[0])) { 
            unique_ptr<BinaryOperator<T>> op;
            switch(value[0]) {
                case '+':
                    op = make_unique<AdditionOperation<T>>(1);
                    break;
                case '-':
                    op = make_unique<MinusOperation<T>>(1);
                    break;
                case '*':
                    op = make_unique<MultiplicationOperation<T>>(2);
                    break;
                case '/':
                    op = make_unique<DivisionOperation<T>>(2);
                    break;
                case '%':
                    op = make_unique<ModuloOperator<T>>(2);
                    break;
                default:
                    throw UnknownOperation();
            }
            while (!stackOperators.empty() && op.get()->getPrecedence() <= stackOperators.top().get()->getPrecedence()) {
                performOperation();
            }
            stackOperators.push(move(op));
        }
        else if (value == "(") {
            stackOperators.push(make_unique<Brackets<T>>());
        } 
        else if (value == ")") {    
            // do the performOperation() method until the opening bracket is not met
            while (!stackOperators.empty() && !(instanceof<Brackets<T>>(stackOperators.top().get()))) { 
                performOperation();
            }
            if (stackOperators.empty()) { 
                throw StackEmptyException();
            }
            stackOperators.pop();   // pop '('
        }
        else {
            T number = getStringFromType(value);

            stackOperands.push(ValueExpression<T> (number));
        }
    }
    
    while (!stackOperators.empty()) {
        performOperation();
    }

    if (stackOperands.size() != 1) {
        throw IncorrectExpressionFormat();
    }
    ValueExpression<T> v = stackOperands.top();
    stackOperands.pop();

    return v.getValue();
}

template class ExpressionEvaluator<int>;
template class ExpressionEvaluator<float>;
template class ExpressionEvaluator<double>;