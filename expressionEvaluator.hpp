#pragma once

#include "expression.hpp"
#include "expressionParser.hpp"

template <class T>
class ExpressionEvaluator { 
    private:
        stack<ValueExpression<T>> stackOperands;
        stack<unique_ptr<BinaryOperator<T>>> stackOperators;

        void performOperation();
        const T getStringFromType(const string& value) const;
    public:
        const T process(const vector<string>& expression);
};
