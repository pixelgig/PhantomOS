# Android companion scope

The embedded browser dashboard is the supported development path for
manual connection and local reports. Open the included preview file to
evaluate the interface without connecting a board. A native companion
would complement the dashboard when optional phone features are useful.

## First native milestone

Use an explicit board connection, foreground requests, connection
deadlines and a local report export. Start with a board already joined by
the user in Settings. Verify the app can reach the Phantom AP while a
phone retains mobile data. If the app manages the local-only Wi-Fi
connection, Android provides `WifiNetworkSpecifier` from API 29.
Concurrency with another Wi-Fi network depends on the device. For target
SDK 37 and above, verify the Android local-network runtime permission and
denial behavior.

Treat BLE, NFC tag reading and USB host as optional capabilities with
their own user actions and platform gates. USB host communication requires
per-device consent. Keep the base application usable on phones lacking
those capabilities. Complete toolchain setup before enabling Android CI.
Record the pinned JDK, Gradle, Android SDK and APK hash. Test an actual
Wi-Fi connection and each optional interface before claiming support.

No native app or APK is part of the current development checkpoint.

- [Android Wi-Fi network specifier](https://developer.android.com/reference/android/net/wifi/WifiNetworkSpecifier)
- [Android local-network permission](https://developer.android.com/privacy-and-security/local-network-permission)
- [USB host](https://developer.android.com/develop/connectivity/usb/host)
- [NFC basics](https://developer.android.com/develop/connectivity/nfc/nfc)
