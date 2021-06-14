#pragma once

/**
 * Types of connections that are possible when creating the initial network connection
 *    Each type has its own implementation function which is called by connectNodeToNetwork
 *      Full = fully connected graph
 *      Partial = randomly connected graph (dependent on Config.visiblePeer's percentage connected)
 *      Single = randomly connected MST (one connection only on creation)
 */
enum class ConnectionType { Full, Partial, Single, Custom, Invalid };

static std::string ConnectionType_toString(ConnectionType type) {
    switch (type) {
        case ConnectionType::Full: return "full";
        case ConnectionType::Partial: return "partial";
        case ConnectionType::Single: return "single";
        case ConnectionType::Custom: return "custom";
        case ConnectionType::Invalid: return "invalid";
    }
}

static ConnectionType ConnectionType_fromString(std::string type) {
  if (type == "full") {
    return ConnectionType::Full;
  } else if (type == "partial") {
    return ConnectionType::Partial;
  } else if (type == "single") {
    return ConnectionType::Single;
  } else if (type == "custom") {
    return ConnectionType::Custom;
  } else {
    return ConnectionType::Invalid;
  }
}
