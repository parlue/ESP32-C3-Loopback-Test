from pathlib import Path
import hashlib, json, subprocess, sys
root = Path(__file__).resolve().parents[1]
subprocess.run([sys.executable, "-m", "platformio", "run", "-e", "loopback"], cwd=root, check=True)
from platformio.project.config import ProjectConfig
packages = Path(ProjectConfig(str(root / "platformio.ini")).get("platformio", "packages_dir"))
build = root / ".pio/build/loopback"
target = root / "docs/firmware/loopback-merged.bin"
subprocess.run([sys.executable, str(packages / "tool-esptoolpy/esptool.py"),
    "--chip", "esp32c3", "merge_bin", "-o", str(target),
    "--flash_mode", "dio", "--flash_freq", "80m", "--flash_size", "4MB",
    "0x0", str(build / "bootloader.bin"), "0x8000", str(build / "partitions.bin"),
    "0xe000", str(packages / "framework-arduinoespressif32/tools/partitions/boot_app0.bin"),
    "0x10000", str(build / "firmware.bin")], check=True)
(target.parent / "SHA256SUMS").write_text(hashlib.sha256(target.read_bytes()).hexdigest() + "  " + target.name + "\n")
print("Web firmware prepared:", target)

