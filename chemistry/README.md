
# Chemical Equation Balancer

The Chemical Equation Balancer is a Python application that helps balance chemical equations using linear programming techniques. It takes an unbalanced chemical equation as input and outputs the balanced equation.

## Features

- Parses chemical equations to extract reactants and products.
- Balances chemical equations using linear programming with the `pulp` library.
- Provides a user-friendly interface using Streamlit.

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/T-droid/chemical-equation-balancer.git
    ```

2. Install dependencies:

    ```bash
    pip install -r requirements.txt
    ```

3. Run the application:

    ```bash
    streamlit run app.py
    ```

## Usage

1. Enter a chemical equation in the input field.
2. Click the "Balance" button to balance the equation.
3. The balanced equation will be displayed below the input field.

## Example

Input: `Al2(CO3)3 + H3PO4 -> AlPO4 + CO2 + H2O`

Output: `3Al2(CO3)3 + 2H3PO4 -> 2AlPO4 + 3CO2 + 3H2O`

## Testing

To run unit tests:

```bash
python test_chemical_equation.py
```
