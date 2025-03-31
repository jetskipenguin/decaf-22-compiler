#!/bin/bash

# Build the project
cd workdir
./build.sh
cd ..

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

# Initialize array for failed tests
failed_tests=()

# Run scanner tests
for frag_file in samples/scanner/*.frag; do
    base_name=$(basename "$frag_file" .frag)
    out_file="samples/scanner/${base_name}.out"
    
    echo "Testing $frag_file..."
    
    # Run the program and capture output
    ./workdir/decaf-22-compiler "$frag_file" "--testScanner" > "temp.out"
    
    # Compare with expected output
    if diff -w "temp.out" "$out_file" > /dev/null; then
        echo "✓ Test passed: $base_name"
    else
        echo "✗ Test failed: $base_name"
        echo "Differences found:"
        diff -w "temp.out" "$out_file"
        failed_tests+=("$frag_file")
    fi
done

# Run syntax analyzer tests
for frag_file in samples/syntax_analyzer/*.decaf; do
    base_name=$(basename "$frag_file" .decaf)
    out_file="samples/syntax_analyzer/${base_name}.out"
    
    echo "Testing $frag_file..."
    
    # Run the program and capture output
    ./workdir/decaf-22-compiler "$frag_file" > "temp.out"
    
    # Compare with expected output
    if diff -w "temp.out" "$out_file" > /dev/null; then
        echo "✓ Test passed: $base_name"
    else
        echo "✗ Test failed: $base_name"
        echo "Differences found:"
        diff -w "temp.out" "$out_file"
        failed_tests+=("$frag_file")
    fi
done


# Print summary of failed tests
if [ ${#failed_tests[@]} -ne 0 ]; then
    echo -e "\nFailed tests:"
    for test in "${failed_tests[@]}"; do
        echo "- $test"
    done
    exit 1
else
    echo -e "\nAll tests passed!"
fi

# Cleanup
rm -f temp.out