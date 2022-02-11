#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <exception>
#include <algorithm>
#include <regex>
#include <stack>
#include <variant>
#include <memory>

using namespace std;

template<typename Base, typename T>
bool instanceof(const T* ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

struct DivisionByZero : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};


struct StackEmptyException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};
 
struct IncorrectExpressionFormat : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct ModuloException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

template <class T>
class ValueExpression {
    private:
        T value;
    public:
        ValueExpression()=default;
        ValueExpression(T value) : value(value) {};
        ~ValueExpression()=default;
        inline T getValue() { return value; }
};

template <class T>
class BinaryOperator { 
    protected:
        size_t precedence; 
    public:
        BinaryOperator()=default;
        BinaryOperator(size_t precedence) : precedence(precedence) {};
        virtual ~BinaryOperator()=default;

        inline size_t getPrecedence() const { return precedence; }
        virtual T evaluate(T valOne, T valTwo) { return (valOne - valTwo) * 0; }
        virtual void toString() const { cout << "Binary operator" << endl; }
};

template <class T>
class Brackets : public BinaryOperator<T> {
    public:
        ~Brackets()=default;
        inline T evaluate(T valOne, T valTwo) override { return (valOne - valTwo) * 0; }
        inline void toString() const override { cout << "Bracket" << endl; }
};

template <class T>
class MinusOperation : public BinaryOperator<T> {
    public:
        MinusOperation(size_t precedence) : BinaryOperator<T>(precedence) {};
        ~MinusOperation()=default;
        inline T evaluate(T valOne, T valTwo) override { return valOne - valTwo; }
        inline void toString() const override { 
            cout << "Minus Operator with precedence: " << this->precedence << endl; 
        }

};

template <class T>
class AdditionOperation : public BinaryOperator<T> {
    public:
        AdditionOperation(size_t precedence) : BinaryOperator<T>(precedence) {};
        ~AdditionOperation()=default;
        inline T evaluate(T valOne, T valTwo) override { return valOne + valTwo; }
        inline void toString() const override { 
            cout << "Plus Operator with precedence: " << this->precedence << endl; 
        }
        
};

template <class T>
class MultiplicationOperation : public BinaryOperator<T> {
    public:
        MultiplicationOperation(size_t precedence) : BinaryOperator<T>(precedence) {};
        ~MultiplicationOperation()=default;
        inline T evaluate(T valOne, T valTwo) override { return valOne * valTwo; }
        inline void toString() const override { 
            cout << "Multiplication Operator with precedence: " << this->precedence << endl; 
        }
};

template <class T>
class DivisionOperation : public BinaryOperator<T> {
    public:
        DivisionOperation(size_t precedence) : BinaryOperator<T>(precedence) {};
        ~DivisionOperation()=default;
        inline T evaluate(T valOne, T valTwo) override { 
            if (valTwo == 0) { throw DivisionByZero(); }
            return valOne / valTwo; 
        }
        inline void toString() const override {
            cout << "Division Operator with precedence: " << this->precedence << endl; 
        }
};

template <class T>
class ModuloOperator : public BinaryOperator<T> {
    public:
        ModuloOperator(size_t precedence) : BinaryOperator<T>(precedence) {};
        ~ModuloOperator()=default;
        inline T evaluate(T valOne, T valTwo) override { 
            if (valTwo == 0 || is_same_v<T, float> || is_same_v<T, double>) { 
                throw DivisionByZero(); 
            }
            return (int)valOne % (int)valTwo; 
        }
        inline void toString() const override {
            cout << "Modulo Operator with precedence: " << this->precedence << endl; 
        }
};

