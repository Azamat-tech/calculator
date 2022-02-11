#pragma once

#include "expressionEvaluator.hpp"

struct VariableAssignmentException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct NumberFormatException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct VariableNameFormat : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct LoadingVariableException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct UnknownOperation : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct MultipleVariableAssignmentException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

struct VariableNotFoundException : public exception {
    const char * what () const throw () {
        return "invalid";
    }
};

class ExpressionParser {
    private:
        vector<string> parsedExpression;
        inline static char operators[5] = {'-', '+', '*', '/', '%'};

        unordered_map<string, variant<int, float, double>> var_storage;

        bool CheckInt(const string& v) const;
        bool CheckFloat(const string& v) const;

        int getNumberOfOccurances(char ch, const string& entity) const;
        bool isBracket(char ch) const;
        bool isFloat(const string& v) const;
        bool isNumber(const string& n) const;
        bool isVarAssignmentCorrect(const string& assignment) const;
        bool containsAlpha(const string& var_name) const;
        bool checkForMultipleDots(const string& entity) const ;
        void parseVariableAssignment(string& expression, string& var_name, string& expr);
        void processEntity(string& entity);

        void convertToString(string& toString, const variant<int, float, double>& v);
        void storeVariable(string& var_name, const variant<int, float, double>& value);

        void changeVariableToValues();
        void getMinusPositions(vector<size_t>& unaryPos);
        void addImpliedValues();
        
        size_t getFirstValue() const;
        size_t getHighestType();

        void parseToVector(const string& expression);

        void print() const;
    public:
        static bool isOperator(char ch);

        void emptyParsedExpression();
        void parseAndEval(string& expression, variant<int, float, double>& output);
        void evaluate(variant<int, float, double>& output);
};