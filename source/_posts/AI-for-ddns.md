---
title: AI for ddns
tags:
  - ai
  - Cloudflare
categories:
  - nas
date: 2026-02-18 16:00:36
---


多嘴说一句 AI 真王朝了。

家里的 NAS 不知道抽什么疯了，IP 一直在变，之前给的固定 IP 不知道为什么没了。得弄 ddns 了。
用的小米路由器，抓一下 IP 然后用 API 更新一下 DNS 记录。

<!-- more -->

AI 写的

```python
import requests
import json
import time
import logging
import re
import random
from typing import Optional, Dict, Any
from pathlib import Path
import hashlib


class MiWiFi:
    def __init__(self, host, username, password, key=None, device_id=None):
        self.host = host
        self.username = username
        self.password = password
        self.key = key
        self.device_id = device_id
        self.token = None
        self._web_res = None
        self._setup_logging(logging.DEBUG)

    def _setup_logging(self, level: int):
        self.logger = logging.getLogger("MiWiFi")
        self.logger.setLevel(level)
        self.logger.handlers.clear()

        ch = logging.StreamHandler()
        ch.setLevel(level)
        ch.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))

        self.logger.addHandler(ch)

    @classmethod
    def from_config(cls, config):
        return cls(
            host=config['host'],
            username=config['username'],
            password=config['password'],
            key=config.get('key'),
            device_id=config.get('device_id')
        )

    @property
    def login_url(self):
        return f"http://{self.host}/cgi-bin/luci/api/xqsystem/login"

    @property
    def web_url(self):
        return f"http://{self.host}/cgi-bin/luci/web"

    def start(self):
        try:
            self.logger.info(f"Starting authentication for {self.host}")
            self.token = self.get_token()
            self.logger.info("Authentication successful")
        except Exception as e:
            self.logger.error(f"Authentication failed: {e}")
            raise

    def _fetch_web_page(self, force=False):
        if self._web_res is None or force:
            try:
                self.logger.debug(
                    f"Fetching web page from {self.web_url}")
                self._web_res = requests.get(self.web_url, timeout=5)
                self._web_res.raise_for_status()
                self.logger.debug("Web page fetched successfully")
            except requests.RequestException as e:
                self.logger.error(f"Failed to fetch web page: {e}")
                raise

    def get_key(self):
        if self.key:
            self.logger.debug("Using pre-configured key")
            return self.key
        self._fetch_web_page()
        match = re.search(r"key\s*:\s*'([^']*)'", self._web_res.text)
        if not match:
            self.logger.error("Failed to extract 'key' from web page")
            raise ValueError("Key extraction failed")
        self.key = match.group(1)
        self.logger.debug(f"Extracted key: {self.key[:6]}...")
        return self.key

    def get_device_id(self):
        if self.device_id:
            self.logger.debug("Using pre-configured device_id")
            return self.device_id
        self._fetch_web_page()
        match = re.search(
            r"deviceId\s*=\s*['\"]([^'\"]+)['\"]", self._web_res.text)
        if not match:
            self.logger.error(
                "Failed to extract 'device_id' from web page")
            raise ValueError("Device ID extraction failed")
        self.device_id = match.group(1)
        self.logger.debug(f"Extracted device_id: {self.device_id}")
        return self.device_id

    def generate_nonce(self):
        try:
            self._fetch_web_page(True)
            if not self.key:
                self.key = self.get_key()
            if not self.device_id:
                self.device_id = self.get_device_id()
            nonce = f"0_{self.device_id}_{int(time.time())}_{random.randint(1000, 10000)}"
            self.logger.debug(f"Generated nonce: {nonce}")
            return nonce
        except Exception as e:
            self.logger.error(f"Nonce generation failed: {e}")
            raise

    def hash_password(self, nonce: str):
        try:
            sha1 = hashlib.sha1(
                (self.password + self.key).encode()).hexdigest()
            hashed = hashlib.sha1((nonce + sha1).encode()).hexdigest()
            self.logger.debug("Password hashed successfully")
            return hashed
        except Exception as e:
            self.logger.error(f"Password hashing failed: {e}")
            raise

    @property
    def login_data(self):
        nonce = self.generate_nonce()
        return {
            "username": self.username,
            "password": self.hash_password(nonce),
            "logtype": 2,
            "nonce": nonce
        }

    def get_token(self):
        try:
            self.logger.info(f"Logging in to {self.host}")
            data = self.login_data
            res = requests.post(self.login_url, data=data, timeout=5)
            self.logger.debug(
                f"Login response status: {res.status_code}")
            if res.status_code != 200:
                self.logger.error(
                    f"Login failed with status {res.status_code}: {res.text}")
                raise Exception(f"Login failed: {res.status_code}")
            result = res.json()
            token = result.get("token")
            if not token:
                self.logger.error(
                    f"Token not found in response: {result}")
                raise ValueError("Token missing in login response")
            self.logger.info("Token acquired successfully")
            return token
        except (requests.RequestException, json.JSONDecodeError, ValueError) as e:
            self.logger.error(f"Token acquisition error: {e}")
            raise

    def api_endpoint(self, endpoint):
        url = f"http://{self.host}/cgi-bin/luci/;stok={self.token}/api/{endpoint}"
        self.logger.debug(f"Requesting API endpoint: {endpoint}")
        try:
            res = requests.get(url, timeout=5)
            self.logger.debug(f"API response status: {res.status_code}")
            if res.status_code != 200:
                self.logger.error(
                    f"API request failed [{endpoint}]: {res.status_code} - {res.text}")
                raise Exception(f"API request failed: {res.status_code}")
            return res.json()
        except requests.RequestException as e:
            self.logger.error(f"API request exception [{endpoint}]: {e}")
            raise

    @property
    def wan_info(self):
        return self.api_endpoint("xqnetwork/wan_info")

    @property
    def public_ip(self):
        try:
            info = self.wan_info
            ip = info["info"]["ipv4"][0]["ip"]
            self.logger.info(f"Public IP retrieved: {ip}")
            return ip
        except (KeyError, IndexError) as e:
            self.logger.error(f"Failed to extract public IP: {e}")
            raise


class CloudflareDDNS:
    IP_SERVICES = [
        "https://api.ipify.org",
        "https://ipv4.icanhazip.com",
        "https://ifconfig.me/ip",
        "https://ip.sb"
    ]

    def __init__(
        self,
        email: str,
        api_key: str,
        zone_id: str,
        dns_record_id: str,
        ip_file: str,
        log_level: int = logging.DEBUG
    ):
        self.email = email
        self.api_key = api_key
        self.zone_id = zone_id
        self.dns_record_id = dns_record_id
        self.ip_file = Path(ip_file)
        self._setup_logging(log_level)
        self.session = requests.Session()
        self.session.headers.update({
            "X-Auth-Email": self.email,
            "X-Auth-Key": self.api_key,
            "Content-Type": "application/json"
        })
        self.api_base = "https://api.cloudflare.com/client/v4"

    @classmethod
    def from_config(cls, config):
        return cls(
            email=config['email'],
            api_key=config['api_key'],
            zone_id=config['zone_id'],
            dns_record_id=config['dns_record_id'],
            ip_file=config.get('ip_file', "/tmp/cloudflare-ddns-ip.txt")
        )

    def add_miwifi(self, miwifi: MiWiFi):
        self.miwifi = miwifi

    def _setup_logging(self, level: int):
        self.logger = logging.getLogger("CloudflareDDNS")
        self.logger.setLevel(level)
        self.logger.handlers.clear()

        ch = logging.StreamHandler()
        ch.setLevel(level)
        ch.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))

        self.logger.addHandler(ch)

    def _validate_ip(self, ip: str) -> bool:
        pattern = r'^(\d{1,3}\.){3}\d{1,3}$'
        if not re.match(pattern, ip):
            return False
        return all(0 <= int(part) <= 255 for part in ip.split('.'))

    def get_public_ip(self) -> str:
        try:
            if self.miwifi is None:
                raise ValueError("MiWiFi instance not set")
            self.miwifi.start()
            public_ip = self.miwifi.public_ip
            if not self._validate_ip(public_ip):
                self.logger.warning(f"Invalid IP from MiWiFi: {public_ip}")
                raise ValueError("Invalid IP format")
            return public_ip
        except Exception as e:
            self.logger.warning(f"Failed to get IP from MiWiFi: {e}")
            return self.get_public_ip_from_services()

    def get_public_ip_from_services(self) -> str:
        self.logger.debug("Starting public IP detection...")
        for url in self.IP_SERVICES:
            try:
                self.logger.debug(f"Trying IP service: {url}")
                resp = self.session.get(url, timeout=5)
                resp.raise_for_status()
                ip = resp.text.strip()
                if self._validate_ip(ip):
                    self.logger.info(f"Public IP retrieved: {ip}")
                    return ip
                self.logger.warning(f"Invalid IP format from {url}: {ip}")
            except Exception as e:
                self.logger.debug(f"Failed to get IP from {url}: {e}")
        self.logger.error("All IP services failed")
        raise RuntimeError("Failed to retrieve public IPv4 address")


    def read_last_ip(self) -> Optional[str]:
        if not self.ip_file.exists():
            self.logger.debug(f"IP file not found: {self.ip_file}")
            return None
        try:
            ip = self.ip_file.read_text().strip()
            if ip and self._validate_ip(ip):
                self.logger.debug(f"Last recorded IP: {ip}")
                return ip
            self.logger.warning(f"Invalid IP in file: {ip}")
            return None
        except Exception as e:
            self.logger.error(f"Failed to read IP file: {e}")
            return None

    def write_current_ip(self, ip: str):
        try:
            self.ip_file.parent.mkdir(parents=True, exist_ok=True)
            self.ip_file.write_text(ip.strip())
            self.logger.debug(f"IP saved to {self.ip_file}: {ip}")
        except Exception as e:
            self.logger.error(f"Failed to write IP file: {e}")
            raise

    def get_dns_record(self) -> Dict[str, Any]:
        url = f"{self.api_base}/zones/{self.zone_id}/dns_records/{self.dns_record_id}"
        try:
            self.logger.debug(f"Fetching DNS record: {self.dns_record_id}")
            resp = self.session.get(url, timeout=10)
            resp.raise_for_status()
            data = resp.json()
            if not data.get("success"):
                raise ValueError(f"API error: {data.get('errors')}")
            record = data["result"]
            self.logger.debug(f"DNS record retrieved: {record['name']} -> {record['content']}")
            return record
        except Exception as e:
            self.logger.error(f"Failed to fetch DNS record: {e}")
            raise

    def update_dns_record(self, new_ip: str) -> bool:
        try:
            record = self.get_dns_record()
            if record["content"] == new_ip:
                self.logger.info(f"IP unchanged in Cloudflare: {new_ip}")
                return True

            payload = {
                "type": record["type"],
                "name": record["name"],
                "content": new_ip,
                "ttl": record.get("ttl", 1),
                "proxied": record.get("proxied", False)
            }

            url = f"{self.api_base}/zones/{self.zone_id}/dns_records/{self.dns_record_id}"
            self.logger.info(f"Updating DNS record: {record['name']} -> {new_ip}")
            resp = self.session.put(url, data=json.dumps(payload), timeout=10)
            resp.raise_for_status()
            result = resp.json()
            if not result.get("success"):
                raise ValueError(f"Update failed: {result.get('errors')}")

            self.logger.info(f"DNS record updated successfully: {new_ip}")
            return True
        except Exception as e:
            self.logger.error(f"DNS update failed: {e}")
            return False

    def run(self) -> bool:
        self.logger.info("=== Starting Cloudflare DDNS Update ===")
        try:
            current_ip = self.get_public_ip()

            last_ip = self.read_last_ip()
            if last_ip == current_ip:
                self.logger.info(f"No IP change detected: {current_ip}")
                return True

            if not self.update_dns_record(current_ip):
                return False

            self.write_current_ip(current_ip)
            self.logger.info("=== DDNS Update Completed Successfully ===")
            return True
        except Exception as e:
            self.logger.exception(f"DDNS update failed: {e}")
            return False

if __name__ == "__main__":
    config_file = "/home/franzero/my_py_script/ddns/config.json"
    with open(config_file, "r") as f:
        config = json.load(f)
    ddns = CloudflareDDNS.from_config(config["cloudflare"])
    miwifi = MiWiFi.from_config(config["MiWiFi"])
    ddns.add_miwifi(miwifi)
    ddns.run()
```

感觉还是 cpp 顺眼一点点

改了一下 CPP 的版本
[cloudflare ddns](https://github.com/Mrwatermolen/CloudflareDDNS)
