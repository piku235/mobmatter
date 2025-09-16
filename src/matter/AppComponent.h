#pragma once

namespace mobmatter::matter {

class AppComponent {
public:
    virtual ~AppComponent() = default;
    virtual void boot() {};
    virtual void run() {};
    virtual void shutdown() {};
};

}
