import random
import argparse

def modify_sequence(sequence, num_changes):
    """
    Modify a given DNA sequence by changing `num_changes` base pairs.
    Changes are made by randomly selecting positions and changing the base to another random base.

    Args:
    sequence (str): The original DNA sequence (A, T, C, G).
    num_changes (int): The number of base pairs to modify.

    Returns:
    str: The modified sequence.
    """
    bases = ['A', 'T', 'C', 'G']
    sequence = list(sequence)  # Convert the sequence to a list of characters
    
    for _ in range(num_changes):
        position = random.randint(0, len(sequence) - 1)  # Pick a random position in the sequence
        current_base = sequence[position]
        new_base = random.choice([base for base in bases if base != current_base])  # Choose a different base
        sequence[position] = new_base  # Modify the base at the position

    return ''.join(sequence)  # Return the modified sequence as a string


def main():
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Modify a DNA sequence by changing a specified number of base pairs.")
    parser.add_argument('num_changes', type=int, help="The number of base pairs to modify in the sequence.")
    args = parser.parse_args()

    # Input and output file names
    input_file = 'input_scramble.txt'
    output_file = 'output_scramble.txt'

    # Read the original sequence from the input file
    with open(input_file, 'r') as f:
        sequence = f.read().strip()  # Strip any extra whitespace or newline characters

    # Modify the sequence
    modified_sequence = modify_sequence(sequence, args.num_changes)

    # Write the modified sequence to the output file
    with open(output_file, 'w') as f:
        f.write(modified_sequence)

    print(f"Original sequence: {sequence}")
    print(f"Modified sequence: {modified_sequence}")
    print(f"Modified sequence has been saved to {output_file}")


if __name__ == "__main__":
    main()