#!/usr/bin/env python3
"""Test script to verify embedded Python is working"""
import sys
import os

print(f"Python executable: {sys.executable}")
print(f"Python version: {sys.version}")
print(f"Working directory: {os.getcwd()}")

# Test requests library
try:
    import requests
    print(f"Requests library: {requests.__version__} - OK")
except ImportError as e:
    print(f"Requests library: FAILED - {e}")
    sys.exit(1)

print("\nAll checks passed!")
sys.exit(0)
