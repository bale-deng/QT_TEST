#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Test script to simulate timeout scenario
This script intentionally hangs to test the 30-second timeout mechanism
"""

import json
import sys
import time

def main():
    """
    Simulate a hanging process that exceeds the 30-second timeout.
    This tests the timeout and process termination mechanism.
    """
    if len(sys.argv) < 2:
        error_output = {
            "status": "error",
            "error_message": "Usage: python test_timeout.py <city_name>"
        }
        print(json.dumps(error_output))
        sys.exit(1)

    city = sys.argv[1]

    # Print initial message
    print(f"Starting timeout test for city: {city}", file=sys.stderr)
    sys.stderr.flush()

    # Simulate a hanging process (e.g., DNS timeout, network hang)
    # This should trigger the 30-second timeout in the Qt application
    print("Simulating network hang...", file=sys.stderr)
    sys.stderr.flush()

    try:
        # Sleep for 60 seconds (longer than the 30-second timeout)
        # The Qt app should terminate this process after 30 seconds
        for i in range(60):
            time.sleep(1)
            if i % 5 == 0:
                print(f"Still hanging... {i} seconds elapsed", file=sys.stderr)
                sys.stderr.flush()

        # This should never be reached if timeout works correctly
        print("ERROR: Process was not terminated by timeout!", file=sys.stderr)
        error_output = {
            "status": "error",
            "error_message": "Timeout test failed - process should have been terminated"
        }
        print(json.dumps(error_output))

    except KeyboardInterrupt:
        # This might be triggered by terminate() signal
        print("Process interrupted (expected behavior)", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
