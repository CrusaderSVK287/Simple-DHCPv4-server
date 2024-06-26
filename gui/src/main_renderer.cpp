#include "main_renderer.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <string>
#include <vector>
#include "ftxui/component/component.hpp" 
#include "ftxui/component/component_base.hpp" 
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"              // for Event, Event::Custom
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp" 
#include "tab_help.hpp"
#include "version_info.hpp"

#include "tab_logs.hpp"
#include "tab_leases.hpp"
#include "tab_inspect.hpp"
#include "tab_config.hpp"
#include "tab_command.hpp"
#include "logger.hpp"

using namespace ftxui;

// static Component _not_yet_implemented_tab(std::string s)
// {
//     return Renderer([s] {
//             return text(std::format("Tab {} not yet implemented", s)) | bold | hcenter;
//         });
// }

int tui_loop()
{
    auto screen = ScreenInteractive::Fullscreen();

    int tab_index = 0;
    std::vector<std::string> tab_entries = {
        " Help ", " Config ", " Logs ", " Leases ", " Inspect ", " Command "
    };

    TabLogs tab_logs = TabLogs();
    TabLease tab_lease = TabLease();
    TabInspect tab_inspect = TabInspect();
    TabConfig tab_config = TabConfig();
    TabCommand tab_command = TabCommand();
    TabHelp tab_help = TabHelp();

    auto tab_selection = Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated()) | hcenter;
    auto tab_contents = Container::Tab({
        tab_help.tab_contents,
        tab_config.tab_contents,
        tab_logs.tab_contents,
        tab_lease.tab_contents,
        tab_inspect.tab_contents,
        tab_command.tab_contents,
        },
        &tab_index);

    auto exit_button = Button("Exit", [&]{screen.Exit();});
    auto main_container = Container::Vertical({
        Container::Horizontal({
            tab_selection,
            exit_button
        }),
        tab_contents,
    });
    
    auto main_renderer = Renderer(main_container, [&] {
        switch (tab_index) {
            case 0: break;
            case 1: tab_config.refresh(); break;
            case 2: tab_logs.refresh(); break;
            case 3: tab_lease.refresh(); break;
            case 4: tab_inspect.refresh(); break;
            case 5: tab_command.refresh(); break;
            default:
                break;
        }

        return vbox({
            hbox({
                text(TUI_VERSION) | bold | flex,
                text("DHCP config") | align_right | flex | bold,
            }),
            hbox({
                tab_selection->Render() | flex,
                exit_button->Render()
            }),
            tab_contents->Render() | flex,
        });
    });

    screen.Loop(main_renderer);

    return 0;
}

