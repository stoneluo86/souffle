/*
 * Copyright (c) 2013, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/************************************************************************
 *
 * @file AstComponent.h
 *
 * Defines the class utilized to model a component within the input program.
 *
 ***********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "AstRelation.h"

class ErrorReport;

/**
 * A component type is the class utilized to represent a construct of the form
 *
 *                  name < Type, Type, ... >
 *
 * where name is the name of the component and < Type, Type, ... > is an optional
 * list of type parameters.
 */
class AstComponentType {

    /**
     * The name of the addressed component.
     */
    std::string name;

    /**
     * The list of associated type parameters.
     */
    std::vector<std::string> typeParams;

public:

    /**
     * Creates a new component type based on the given name and parameters.
     */
    AstComponentType(const std::string& name = "", const std::vector<std::string>& params = std::vector<std::string>())
        : name(name), typeParams(params) {}

    // -- copy constructors and assignment operators --

    AstComponentType(const AstComponentType& other) = default;
    AstComponentType(AstComponentType&& other) = default;

    AstComponentType& operator=(const AstComponentType& other) =default;
    AstComponentType& operator=(AstComponentType&& other) =default;


    // -- equality and inequality operators --

    bool operator==(const AstComponentType& other) const {
        return this == &other || (name == other.name && typeParams == other.typeParams);
    }

    bool operator!=(const AstComponentType& other) const {
        return !(*this == other);
    }


    // -- getters and setters --

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& n) {
        name = n;
    }

    const std::vector<std::string>& getTypeParameters() const {
        return typeParams;
    }

    void setTypeParameters(const std::vector<std::string>& params) {
        typeParams = params;
    }


    // -- printers --

    void print(std::ostream& out) const {
        out << name;
        if (!typeParams.empty()) out << "<" << join(typeParams, ",") << ">";
    }

    friend std::ostream& operator<<(std::ostream& out, const AstComponentType& id) {
        id.print(out);
        return out;
    }

};


/**
 * A node type representing expressions utilized to initialize components by
 * binding them to a name.
 */
class AstComponentInit : public AstNode {

    /**
     * The name of the resulting component instance.
     */
    std::string instanceName;

    /**
     * The type of the component to be instantiated.
     */
    AstComponentType componentType;

public:


    // -- getters and setters --

    const std::string& getInstanceName() const {
        return instanceName;
    }

    void setInstanceName(const std::string& name) {
        instanceName = name;
    }

    const AstComponentType& getComponentType() const {
        return componentType;
    }

    void setComponentType(const AstComponentType& type) {
        componentType = type;
    }

    /** Requests an independent, deep copy of this node */
    virtual AstComponentInit* clone() const {
        auto res = new AstComponentInit();
        res->componentType = componentType;
        res->instanceName = instanceName;
        return res;
    }

    /** Applies the node mapper to all child nodes and conducts the corresponding replacements */
    virtual void apply(const AstNodeMapper& mapper) {
        return; // nothing to do
    }

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const {
        std::vector<const AstNode*> res;
        return res;     // no child nodes
    }

    /** Output to a given output stream */
    virtual void print(std::ostream &os) const {
        os << ".init " << instanceName << " = " << componentType;
    }

protected:

    /** An internal function to determine equality to another node */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstComponentInit*>(&node));
        const AstComponentInit& other = static_cast<const AstComponentInit&>(node);
        return instanceName == other.instanceName && componentType == other.componentType;
    }

};


/**
 * A AST node describing a component within the input program.
 */
class AstComponent : public AstNode {

    /**
     * The type of this component, including its name and type parameters.
     */
    AstComponentType type;

    /**
     * A list of base types to inherit relations and clauses from.
     */
    std::vector<AstComponentType> baseComponents;

    /**
     * A list of relations declared in this component.
     */
    std::vector<std::unique_ptr<AstRelation>> relations;

    /**
     * A list of clauses defined in this component.
     */
    std::vector<std::unique_ptr<AstClause>> clauses;

    /**
     * A list of nested components.
     */
    std::vector<std::unique_ptr<AstComponent>> components;

    /**
     * A list of nested component instantiations.
     */
    std::vector<std::unique_ptr<AstComponentInit>> instantiations;

    /**
     * Set of relations that are overwritten 
     */ 
    std::set<std::string> overrideRules; 

public:

    ~AstComponent() { }


    // -- getters and setters --

    const AstComponentType& getComponentType() const {
        return type;
    }

    void setComponentType(const AstComponentType& type) {
        this->type = type;
    }

    const std::vector<AstComponentType>& getBaseComponents() const {
        return baseComponents;
    }

    void setBaseComponents(const std::vector<AstComponentType>& basis) {
        baseComponents = basis;
    }

    void addBaseComponent(const AstComponentType& component) {
        baseComponents.push_back(component);
    }

    void addRelation(std::unique_ptr<AstRelation> r) {
        relations.push_back(std::move(r));
    }

    std::vector<AstRelation *> getRelations() const {
        return toPtrVector(relations);
    }

    void addClause(std::unique_ptr<AstClause> c) {
        clauses.push_back(std::move(c));
    }

    std::vector<AstClause *> getClauses() const {
        return toPtrVector(clauses);
    }

    void addComponent(std::unique_ptr<AstComponent> c) {
        components.push_back(std::move(c));
    }

    std::vector<AstComponent *> getComponents() const {
        return toPtrVector(components);
    }

    void addInstantiation(std::unique_ptr<AstComponentInit> i) {
        instantiations.push_back(std::move(i));
    }

    std::vector<AstComponentInit *> getInstantiations() const {
        return toPtrVector(instantiations);
    }

    void addOverride(const std::string &name) { 
        overrideRules.insert(name); 
    }

    const std::set<std::string> &getOverridden() const {
        return overrideRules;
    }

    /** Requests an independent, deep copy of this node */
    virtual AstComponent* clone() const {
        AstComponent* res = new AstComponent();

        res->setComponentType(getComponentType());
        res->setBaseComponents(getBaseComponents());

        for(const auto& cur : components)       res->components.push_back(std::unique_ptr<AstComponent>(cur->clone()));
        for(const auto& cur : instantiations)   res->instantiations.push_back(std::unique_ptr<AstComponentInit>(cur->clone()));
        for(const auto& cur : relations)        res->relations.push_back(std::unique_ptr<AstRelation>(cur->clone()));
        for(const auto& cur : clauses)          res->clauses.push_back(std::unique_ptr<AstClause>(cur->clone()));
        for(const auto& cur : overrideRules)    res->overrideRules.insert(cur);

        return res;
    }

    /** Applies the node mapper to all child nodes and conducts the corresponding replacements */
    virtual void apply(const AstNodeMapper& mapper) {

        // apply mapper to all sub-nodes
        for(auto& cur : components)       cur = mapper(std::move(cur));
        for(auto& cur : instantiations)   cur = mapper(std::move(cur));
        for(auto& cur : relations)        cur = mapper(std::move(cur));
        for(auto& cur : clauses)          cur = mapper(std::move(cur));

        return;
    }

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const {
        std::vector<const AstNode*> res;

        for(const auto& cur : components)       res.push_back(cur.get());
        for(const auto& cur : instantiations)   res.push_back(cur.get());
        for(const auto& cur : relations)        res.push_back(cur.get());
        for(const auto& cur : clauses)          res.push_back(cur.get());

        return res;
    }

    /** Output to a given output stream */
    virtual void print(std::ostream &os) const {
        os << ".comp " << getComponentType() << " ";

        if (!baseComponents.empty()) {
            os << ": " << join(baseComponents, ",") << " ";
        }
        os << "{\n";

        if (!components.empty())        os << join(components, "\n", print_deref<std::unique_ptr<AstComponent>>()) << "\n";
        if (!instantiations.empty())    os << join(instantiations, "\n", print_deref<std::unique_ptr<AstComponentInit>>()) << "\n";
        if (!relations.empty())         os << join(relations, "\n", print_deref<std::unique_ptr<AstRelation>>()) << "\n";
        for (const auto &cur : overrideRules) { 
            os << ".override " << cur << "\n"; 
        } 
        if (!clauses.empty())           os << join(clauses, "\n\n", print_deref<std::unique_ptr<AstClause>>()) << "\n";

        os << "}\n";
    }

protected:

    /** An internal function to determine equality to another node */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstComponent*>(&node));
        const AstComponent& other = static_cast<const AstComponent&>(node);

        // compare all fields
        return type == other.type && baseComponents == other.baseComponents &&
               equal_targets(relations, other.relations) &&
               equal_targets(clauses, other.clauses) &&
               equal_targets(components, other.components) &&
               equal_targets(instantiations, other.instantiations);
    }

};
