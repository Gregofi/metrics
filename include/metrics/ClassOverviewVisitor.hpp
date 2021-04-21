#ifndef METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP
#define METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "include/ASTMatcherVisitor.hpp"
#include "include/CtxVisitor.hpp"

/**
 * Used on single method of class, traverses method body and collects:
 *  - IDs of classes from which are methods that this class calls (doesn't add current class)
 *  - IDs of instance variables from this class that current method uses
 */
class MethodCallback : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    explicit MethodCallback(std::string currClassName) noexcept : currClassID(std::move(currClassName)) {}
    void SetCurrentFunction(std::string s) { currFunction = std::move(s); }
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    /**
     * Returns ID of classes which this method called, original set is moved.
     * @return
     */
    std::set<std::string> GetClasses() { return std::move(classes); }
    /**
     * Returns ID of instance var which this method used, original set is moved.
     * @return
     */
    std::set<long unsigned> GetInstanceVars() { return std::move(instance_vars); }


protected:
    std::string currFunction{};
    std::string currClassID;
    std::set<std::string> classes;
    std::set<long unsigned> instance_vars;
};

/**
 *  This calculates metrics about class design.
 *  For each class, this calculates:
 *    Chidamber and Kemerer metrics:
 *      - Number of immediate children
 *      - Length of inheritance chain
 *      - Lack of cohesion
 *      - Coupling
 *    Lorenz and Kidd metrics:
 *      - Public member variable count
 *      - Member variable count
 *      - Public not inherited methods
 *      - Not inherited methods
 *      - Overriden methods
 *
 *  It is stored in the structure Class which can be returned. Or all the values can
 *  be returned separately by using the corresponding getters. These classes are identified
 *  by their clang ids.
 */
class ClassOverviewVisitor : public clang::RecursiveASTVisitor<ClassOverviewVisitor>, public CtxVisitor
{
protected:
    struct Class
    {
        /**
         * Number of children for given Key
         */
        int children_count{0};

        /**
         * Contains ID of all function from which this class directly derives.
         */
        std::vector<std::string> inheritance_chain;

        /**
         * Contains classes whose methods this class calls
         */
        std::set<std::string> couples;

        /**
         * Contains sets of integers, each of these sets contains IDs of used instance variables by one method.
         */
        std::vector<std::set<size_t> > functions;

        /**
         * Number of public methods (except default ones)
         */
        int public_methods_count{0};

        /**
         * Number of all methods (except default ones)
         */
        int methods_count{0};

        /**
         * Number of instance variables.
         */
        int instance_vars_count{0};
        int public_instance_vars_count{0};

        /**
         * Number of overriden methods.
         */
        int overriden_methods_count{0};
    };

public:
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);
    const Class& GetRefClass(const std::string &s) const { return classes.at(s); }

    virtual std::ostream &ExportXML(const std::string &s, std::ostream &os) const override;
    /**
     * Returns length of inheritance chain from this class to root class (class that doesn't inherit from any other
     * classes). If there are multiple chains, returns length of the longest one.
     * @param id - ID of the class
     * @return - Lenght of inheritance chain
     */
    int GetInheritanceChainLen(const std::string &s) const;

    /**
     * Returns true if both sets have atleast one same element, otherwise returns false.
     * @param s1
     * @param s2
     * @return
     */
    static bool Similar(const std::set<long unsigned> &s1, const std::set<long unsigned> &s2);

    /**
     * Calculates the 'lack of cohesion' metric for given class.
     *
     * @details The Lack of cohesion calculates how methods use its class instance variables.
     * Two methods are similiar if they use atleast one same instance variable. For all pairs of
     * methods, substract the count of similiar from not similiar.
     *
     * @param id - ID of the class
     * @return - Lack of cohesion metric value
     */
    int LackOfCohesion(const std::string &s) const;

    /**
     * This calculates lorenz and kidd metrics for given class.
     * @param decl - class to calculate the metrics.
     */
    virtual void CalcMetrics(clang::ASTContext *ctx) override;
    virtual std::ostream &Export(const std::string &s, std::ostream &os) const override;
protected:
    void CalculateLorKiddMetrics(clang::CXXRecordDecl *decl);
    std::map<std::string, Class> classes;
};

#endif //METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP
