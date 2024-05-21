#!/usr/bin/env python3
import unittest
from chemistry import parse, balance


class TestChemicalEquationBalancer(unittest.TestCase):
    def test_parse_valid_equation(self):
        equation = "Al2(CO3)3 + H3PO4 -> AlPO4 + CO2 + H2O"
        lhs_compounds, rhs_compounds, unique_elements, all_compounds = parse(equation)
        self.assertEqual(lhs_compounds, ["Al2(CO3)3", "H3PO4"])
        self.assertEqual(rhs_compounds, ["AlPO4", "CO2", "H2O"])
        self.assertEqual(unique_elements, {'Al': '', 'C': '', 'O': '', 'H': '', 'P': ''})


    def test_parse_invalid_equation(self):
        equation = "Al2(CO3)3 + H3PO4 - AlPO4 + CO2 + H2O"
        with self.assertRaises(ValueError):
            parse(equation)



    def test_balance_valid_equation(self):
        equation = "Al2(CO3)3 + H3PO4 -> AlPO4 + CO2 + H2O"
        balanced_equation = balance(equation)
        self.assertEqual(balanced_equation, "1Al2(CO3)3 + 2H3PO4 -> 2AlPO4 + 3CO2 + 3H2O")


    def test_balance_invalid_equation(self):
        equation = "Al2(CO3)3 + H3PO4 - AlPO4 + CO2 + H2O"
        with self.assertRaises(ValueError):
            balance(equation)



if __name__ == '__main__':
    unittest.main()
