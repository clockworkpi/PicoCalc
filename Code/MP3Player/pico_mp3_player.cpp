/////////////////////////////
// MP3 player for RPi Pico //
/////////////////////////////
//
// main() only starts the multitasking and
// runs the main task (see main_task.h/cpp)
//
#include <cstdio>
#include "main_task.h"
#include "uart_rp2040.h"
#include "posix_io.h"
int main()
{
    uart_rp2040 uart;
    posix_io::inst.register_stdin ( uart );
    posix_io::inst.register_stdout( uart );
    posix_io::inst.register_stderr( uart );

    task::sleep_ms(2000);//really neccessary form uart debug
    // Start Main task as privileged task
    main_task Main;
    Main.sign_up(core_t::CURRENT_CORE, 50, true);
    // Start the multitasking
    task::start_scheduler();
}
