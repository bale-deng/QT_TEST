#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Weather Service Test Script with Configurable Delay
Usage: python weather_service_test.py <city_name> [delay_seconds]
"""

import json
import sys
import time
from typing import Dict

def get_weather_with_delay(city: str, delay: int = 0) -> Dict[str, any]:
    """
    Get weather data with an artificial delay to test timeout.

    Args:
        city: The name of the city
        delay: Seconds to delay before returning (default: 0)

    Returns:
        A dictionary containing weather information
    """
    try:
        # Simulate network delay
        if delay > 0:
            print(f"Simulating {delay}s delay...", file=sys.stderr)
            sys.stderr.flush()

            for i in range(delay):
                time.sleep(1)
                if i % 5 == 0 and i > 0:
                    print(f"Delay progress: {i}/{delay} seconds", file=sys.stderr)
                    sys.stderr.flush()

        # Return mock weather data
        weather_data = {
            "city": city,
            "temperature": 20.5,
            "humidity": 65,
            "description": "Test Mode - Partly cloudy",
            "wind_speed": 12.3,
            "feels_like": 19.2,
            "pressure": 1013.25,
            "status": "success"
        }
        return weather_data

    except Exception as e:
        return {
            "city": city,
            "temperature": 0,
            "humidity": 0,
            "description": f"Error: {str(e)}",
            "wind_speed": 0,
            "status": "error",
            "error_message": str(e)
        }

def main():
    """
    Main function for command line interface.
    """
    if len(sys.argv) < 2:
        error_output = {
            "status": "error",
            "error_message": "Usage: python weather_service_test.py <city_name> [delay_seconds]"
        }
        print(json.dumps(error_output))
        sys.exit(1)

    city = sys.argv[1]
    delay = int(sys.argv[2]) if len(sys.argv) > 2 else 0

    print(f"Testing weather service for {city} with {delay}s delay", file=sys.stderr)
    sys.stderr.flush()

    weather_data = get_weather_with_delay(city, delay)

    # Output JSON to stdout for C++ parsing
    print(json.dumps(weather_data))

if __name__ == "__main__":
    main()
