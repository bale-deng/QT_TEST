#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
天气服务模块
为 Qt 应用程序提供天气数据检索功能。
"""

import json
import sys
from typing import Dict, Optional


class WeatherService:
    """
    用于获取和处理天气数据的服务类。
    这是在后续阶段将进行扩展的原型实现。
    """

    def __init__(self):
        """初始化天气服务。"""
        self.api_key = None
        self.base_url = "https://api.openweathermap.org/data/2.5/weather"

    def set_api_key(self, api_key: str) -> None:
        """
        设置天气服务的 API 密钥。

        参数：
            api_key：OpenWeatherMap 或类似服务的 API 密钥
        """
        self.api_key = api_key

    def get_weather(self, city: str) -> Dict[str, any]:
        """
        获取指定城市的天气数据。

        参数：
            city：要获取天气的城市名称

        返回：
            包含天气信息的字典
        """
        try:
            import requests
            import urllib.parse
            
            # 步骤 1：使用地理编码 API 从城市名称获取坐标
            geocode_url = f"https://geocoding-api.open-meteo.com/v1/search?name={urllib.parse.quote(city)}&count=1&language=en&format=json"
            
            response = requests.get(geocode_url, timeout=5)
            response.raise_for_status()
            geo_data = response.json()
            
            if 'results' not in geo_data or len(geo_data['results']) == 0:
                raise Exception(f"未找到城市 '{city}'")
            
            location = geo_data['results'][0]
            lat = location['latitude']
            lon = location['longitude']
            city_name = location['name']
            
            # 步骤 2：使用坐标获取天气数据
            weather_url = f"https://api.open-meteo.com/v1/forecast?latitude={lat}&longitude={lon}&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m,surface_pressure&timezone=auto"
            
            response = requests.get(weather_url, timeout=5)
            response.raise_for_status()
            weather_data_raw = response.json()
            current = weather_data_raw['current']
            
            # 将天气代码映射到描述
            weather_codes = {
                0: "晴朗", 1: "晴间多云", 2: "部分多云", 3: "阴天",
                45: "雾", 48: "沉积雾",
                51: "毛毛雨", 53: "中等毛毛雨", 55: "密集的毛毛雨",
                61: "小雨", 63: "中雨", 65: "大雨",
                71: "小雪", 73: "中雪", 75: "大雪",
                77: "雪粒", 80: "小阵雨", 81: "中等阵雨",
                82: "剧烈阵雨", 85: "小阵雪", 86: "大阵雪",
                95: "雷暴", 96: "雷暴伴有小冰雹", 99: "雷暴伴有大冰雹"
            }
            
            weather_code = current.get('weather_code', 0)
            description = weather_codes.get(weather_code, "未知")
            
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
            # 如果 API 调用失败，返回错误数据
            return {
                "city": city,
                "temperature": 0,
                "humidity": 0,
                "description": f"错误：{str(e)}",
                "wind_speed": 0,
                "status": "error",
                "error_message": str(e)
            }

    def format_weather_data(self, weather_data: Dict[str, any]) -> str:
        """
        将天气数据格式化为可读字符串。

        参数：
            weather_data：包含天气信息的字典

        返回：
            天气数据的格式化字符串表示
        """
        if weather_data.get("status") != "success":
            error_msg = weather_data.get("error_message", "未知错误")
            return f"错误：无法获取天气数据\n{error_msg}"

        formatted = f"""
{weather_data['city']} 天气：
温度：{weather_data['temperature']}°C
体感温度：{weather_data.get('feels_like', '暂无数据')}°C
湿度：{weather_data['humidity']}%
天气情况：{weather_data['description']}
风速：{weather_data['wind_speed']} km/h
气压：{weather_data.get('pressure', '暂无数据')} mb
"""
        return formatted.strip()


def main():
    """
    命令行界面的主函数。
    将 JSON 输出到 stdout 以便 C++ 集成。
    """
    if len(sys.argv) < 2:
        error_output = {
            "status": "error",
            "error_message": "用法：python weather_service.py <城市名称>"
        }
        print(json.dumps(error_output))
        sys.exit(1)

    city = sys.argv[1]
    service = WeatherService()
    weather_data = service.get_weather(city)

    # 仅将 JSON 输出到 stdout 以供 C++ 解析
    print(json.dumps(weather_data))


if __name__ == "__main__":
    main()
