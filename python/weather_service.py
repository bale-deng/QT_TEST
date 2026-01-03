#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Weather Service Module
Provides weather data retrieval functionality for the Qt application.
"""

import json
import sys
from typing import Dict, Optional


class WeatherService:
    """
    A service class to fetch and process weather data.
    This is a prototype implementation that will be extended in later phases.
    """

    def __init__(self):
        """Initialize the weather service."""
        self.api_key = None
        self.base_url = "https://api.openweathermap.org/data/2.5/weather"

    def set_api_key(self, api_key: str) -> None:
        """
        Set the API key for weather service.

        Args:
            api_key: The API key for OpenWeatherMap or similar service
        """
        self.api_key = api_key

    def get_weather(self, city: str) -> Dict[str, any]:
        """
        Get weather data for a specified city.

        Args:
            city: The name of the city to get weather for

        Returns:
            A dictionary containing weather information
        """
        try:
            import requests
            import urllib.parse
            
            # Step 1: Get coordinates from city name using geocoding API
            geocode_url = f"https://geocoding-api.open-meteo.com/v1/search?name={urllib.parse.quote(city)}&count=1&language=en&format=json"
            
            response = requests.get(geocode_url, timeout=5)
            response.raise_for_status()
            geo_data = response.json()
            
            if 'results' not in geo_data or len(geo_data['results']) == 0:
                raise Exception(f"City '{city}' not found")
            
            location = geo_data['results'][0]
            lat = location['latitude']
            lon = location['longitude']
            city_name = location['name']
            
            # Step 2: Get weather data using coordinates
            weather_url = f"https://api.open-meteo.com/v1/forecast?latitude={lat}&longitude={lon}&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m,surface_pressure&timezone=auto"
            
            response = requests.get(weather_url, timeout=5)
            response.raise_for_status()
            weather_data_raw = response.json()
            current = weather_data_raw['current']
            
            # Map weather codes to descriptions
            weather_codes = {
                0: "Clear sky", 1: "Mainly clear", 2: "Partly cloudy", 3: "Overcast",
                45: "Foggy", 48: "Depositing rime fog",
                51: "Light drizzle", 53: "Moderate drizzle", 55: "Dense drizzle",
                61: "Slight rain", 63: "Moderate rain", 65: "Heavy rain",
                71: "Slight snow", 73: "Moderate snow", 75: "Heavy snow",
                77: "Snow grains", 80: "Slight rain showers", 81: "Moderate rain showers",
                82: "Violent rain showers", 85: "Slight snow showers", 86: "Heavy snow showers",
                95: "Thunderstorm", 96: "Thunderstorm with slight hail", 99: "Thunderstorm with heavy hail"
            }
            
            weather_code = current.get('weather_code', 0)
            description = weather_codes.get(weather_code, "Unknown")
            
            weather_data = {
                "city": city_name,
                "temperature": round(current['temperature_2m'], 1),
                "humidity": current['relative_humidity_2m'],
                "description": description,
                "wind_speed": round(current['wind_speed_10m'], 1),
                "feels_like": round(current['apparent_temperature'], 1),
                "pressure": current['surface_pressure'],
                "status": "success"
            }
            return weather_data
                
        except Exception as e:
            # Return error data if API call fails
            return {
                "city": city,
                "temperature": 0,
                "humidity": 0,
                "description": f"Error: {str(e)}",
                "wind_speed": 0,
                "status": "error",
                "error_message": str(e)
            }

    def format_weather_data(self, weather_data: Dict[str, any]) -> str:
        """
        Format weather data into a readable string.

        Args:
            weather_data: Dictionary containing weather information

        Returns:
            Formatted string representation of weather data
        """
        if weather_data.get("status") != "success":
            error_msg = weather_data.get("error_message", "Unknown error")
            return f"Error: Unable to fetch weather data\n{error_msg}"

        formatted = f"""
Weather for {weather_data['city']}:
Temperature: {weather_data['temperature']}°C
Feels Like: {weather_data.get('feels_like', 'N/A')}°C
Humidity: {weather_data['humidity']}%
Conditions: {weather_data['description']}
Wind Speed: {weather_data['wind_speed']} km/h
Pressure: {weather_data.get('pressure', 'N/A')} mb
"""
        return formatted.strip()


def main():
    """
    Main function for command line interface.
    Outputs JSON to stdout for C++ integration.
    """
    if len(sys.argv) < 2:
        error_output = {
            "status": "error",
            "error_message": "Usage: python weather_service.py <city_name>"
        }
        print(json.dumps(error_output))
        sys.exit(1)

    city = sys.argv[1]
    service = WeatherService()
    weather_data = service.get_weather(city)

    # Output only JSON to stdout for C++ parsing
    print(json.dumps(weather_data))


if __name__ == "__main__":
    main()
