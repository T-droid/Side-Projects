#!/usr/bin/env python3
import chemparse as cp
from pulp import *
import streamlit as st

def parse(equation):
    try:
        lhs, rhs = equation.split('->')
    except ValueError:
        raise ValueError("Equation must contain exactly one '->' separating reactants and products.")

    lhsCompounds = lhs.split("+")
    rhsCompounds = rhs.split("+")

    lhsCompounds = [e.strip() for e in lhsCompounds]
    rhsCompounds = [e.strip() for e in rhsCompounds]

    allCompounds = []
    uniqueElements = {}

    for compound in lhsCompounds:
        try:
            numElements = cp.parse_formula(compound)
        except Exception as e:
            raise ValueError(f"Error parsing compound '{compound}' on the left side: {e}")
        for key in numElements:
            uniqueElements[key] = ''
        allCompounds.append(numElements)

    for compound in rhsCompounds:
        try:
            numElements = cp.parse_formula(compound)
            numElements = {key: -val for key, val in numElements.items()}
        except Exception as e:
            raise ValueError(f"Error parsing compound '{compound}' on the right side: {e}")
        allCompounds.append(numElements)

    return lhsCompounds, rhsCompounds, uniqueElements, allCompounds

def balance(equation):
    lhsCompounds, rhsCompounds, uniqueElements, allCompounds = parse(equation)

    # Variables
    variables = []

    for idx, item in enumerate(allCompounds):
        variables.append(LpVariable('x' + str(idx), cat='Integer', lowBound=1))

    # Problem
    prob = LpProblem("Balance Equation", LpMinimize)
    prob += 0, "Objective Function"

    for element in uniqueElements:
        constraint = None
        for idx, compound in enumerate(allCompounds):
            if constraint is None:
                constraint = variables[idx] * compound.get(element, 0)
            else:
                constraint += variables[idx] * compound.get(element, 0)
        prob += constraint == 0, f'Constraint for "{element}"'

    st.text(prob)

    prob.solve()

    if LpStatus[prob.status] != 'Optimal':
        raise ValueError("No solution found. The equation might be impossible to balance.")

    try:
        coeffs = [int(var.value()) for var in variables]
    except TypeError:
        raise TypeError("The equation has a problem, their may be a misplaced character check for spaces in between compounds and misplaced + signs")
    balancedEquation = ""

    balancedCompounds = []
    for coeff, compound in zip(coeffs, lhsCompounds + rhsCompounds):
        balancedCompounds.append(str(coeff) + compound)

    balancedEquation += ' + '.join(balancedCompounds[:len(lhsCompounds)])
    balancedEquation += ' -> '
    balancedEquation += ' + '.join(balancedCompounds[len(lhsCompounds):])
    st.title("Balanced Equation")
    return balancedEquation

def main():
    equation = st.text_input(label="Enter Chemical Equation: A + B -> AB", value="Al2(CO3)3 + H3PO4 -> AlPO4 + CO2 + H2O")
    if st.button("Balance"):
        try:
            balanced_eq = balance(equation)
            st.subheader(balanced_eq)
        except ValueError as e:
            st.error(f"Error: {e}")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        st.error(f"Unexpected error: {e}")
