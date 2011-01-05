## Welcome to Pit ##
Pit is a command-line project manager that integrates with Git. I wrote it
because I needed a command-line tool for tracking tasks. Besides, after
years of Ruby and JavaScript programming I missed plain C.

### Installing Pit ###
Pit is implemented in C and compiles down to a single executable file. It
has been tested to compile on Mac OS Leopard, Ubuntu, and RedHat Linux.

    $ git clone git://github.com/michaeldv/pit.git
    $ cd pit
    $ make
    $ make test     # <-- Optional, requires Ruby
    $ sudo make install # or 'make install PREFIX=${HOME}' to install in ${HOME}/bin
    $ which pit
    /usr/local/bin/pit
    $ pit version
    0.1.0

### Basic Concepts ###
Basic Pit entities are projects, tasks, and notes. One project can have
multiple tasks, and a task can have multiple notes. Each entity has a number
of attributes. For example, project has name and status, task has name,
status, priority, date, and time, and within note there is message body.
All attributes except name and message body are optional and can be omitted.

The attributes have no semantic meaning, and do not have a pre-defined set
of values. For example, depending on the particular need, the time attribute
could be used as projected time in weeks, hours spent on the task, or days
left to finish the task.

Pit tries to maintain a notion of "current" project, task, or note. When you
create new project, it automatically becomes current. If you do not specify
project number when creating a task, the new task will be associated with
the current project.

### Pit Commands ###
Pit commands are as follows:

    init       Create an empty Pit database or re-initialize an existing one
    project    Create, search, and manage Pit projects
    task       Create, search, and manage Pit tasks
    note       Create, search, and manage Pit notes
    log        Show chronological Pit activity log
    info       Show summary information about your Pit database
    help       Show help information about Pit
    version    Show Pit version number

All commands may be shortened, as long as they remain unambiguous. For more
information on a specific command run:

    $ pit help <command> 

### Git Integration ###
Pit distribution comes with tools/commit-msg file. Copy this file to
.git/hooks/commit-msg and make it executable:

    $ cp ~/pit/tools/commit-msg .git/hooks
    $ chmod +x .git/hooks/commit-msg

Create git branch using task number as a branch name. Now on every commit to
the branch the hook will prompt you to update task status. The hook appends
Pit task number to the commit message, updates Pit task status, and creates
task note with the commit massage. For example:

    $ git checkout -b 2
    Switched to a new branch '2'

    $ touch README
    $ git add .
    $ git commit -am "Added README file"
    What is the status of task 2?
      (I)n progress
      (P)ostponed
      (O)pen
      (D)one
    Enter the status for task 2 [D]:
    i
    updated task 2: My second task (status: in progress)
    created note 2: Added README file [task 2, status:in progress] (task 2)
    [2 0d930fb] Added README file [task 2, status:in progress]
     0 files changed, 0 insertions(+), 0 deletions(-)
     create mode 100644 README

### Tips ###
A few tips to get you going:

    # Changing default Pit file name: define PITFILE environment variable.
    $ pit init
    Created empty /Users/mike/.pit

    $ export PITFILE="~/pit.db"
    $ pit init
    Created empty /Users/mike/pit.db

    # Displaying last 10 lines of pit log in reverse order:
    $ pit log|tail -10|sed -n '1!G;h;$p'

    # Displaying tasks within certain date range:
    $ pit task -q -d "Jan 1" -D "Sep 1"

    # Displaying tasks with certain time range:
    $ pit task -q -t 0:30 -T 8:00

### Sample Pit session ###

    $ pit init
    Created empty /Users/mike/.pit

    $ pit project -c "My very first project"
    created project 1: My very first project (status: active)

    $ pit project -c "My second project" -s backlog
    created project 2: My second project (status: backlog)

    $ pit project
      1: (mike) |active | My very first project (0 tasks)
    * 2: (mike) |backlog| My second project     (0 tasks)

    $ pit project -e 1 -s current
    updated project 1: My very first project (status: current)

    $ pit project
    * 1: (mike) |current| My very first project (0 tasks)
      2: (mike) |backlog| My second project     (0 tasks)

    $ pit task -c "My very first task"
    created task 1: My very first task (status: open, priority: normal, project: 1)

    $ pit task -c "My second task" -s new -p high
    created task 2: My second task (status: new, priority: high, project: 1)

    $ pit task -c "My third task" -p low -t 4:00
    created task 3: My third task (status: open, priority: low, time: 4:00, project: 1)

    $ pit p
    * 1: (mike) |current| My very first project (3 tasks)
      2: (mike) |backlog| My second project     (0 tasks)

    $ pit t
      1: (mike) |open| |normal|      My very first task (0 notes)
      2: (mike) |new | |high  |      My second task     (0 notes)
    * 3: (mike) |open| |low   | 4:00 My third task      (0 notes)

    $ pit task -e -s new
    updated task 3: My third task (status: new)

    $ pit task -e 1 -d 10/10
    updated task 1: My very first task (date: Oct 10, 2010)
    * 1: (mike) |open| |normal| Oct 10, 2010      My very first task (0 notes)
      2: (mike) |new | |high  |                   My second task     (0 notes)
      3: (mike) |new | |low   |              4:00 My third task      (0 notes)

    $ pit note -c "Sample note for task #1"
    created note 1: Sample note for task #1 (task 1)

    $ pit task -q -s new
      2: (mike) |new| |high|      My second task (0 notes)
      3: (mike) |new| |low | 4:00 My third task  (0 notes)

    $ pit task -m -p 2
    moved task 1: from project 1 to project 2

    $ pit project 2
      1: (mike) |current| My very first project (2 tasks)
    * 2: (mike) |backlog| My second project     (1 task)    

    $ pit project -d
    deleted note 1: Sample note for task #1 (task 1)
    deleted task 1: My very first task with 1 note (project: 2)
    deleted project 2: My second project with 1 task

    $ pit log
    Aug 22, 2010 14:30 (mike): Initialized pit
    Aug 22, 2010 14:31 (mike): created project 1: My very first project (status: active)
    Aug 22, 2010 14:31 (mike): created project 2: My second project (status: backlog)
    Aug 22, 2010 14:31 (mike): updated project 1: My very first project (status: current)
    Aug 22, 2010 14:31 (mike): created task 1: My very first task (status: open, priority: normal, project: 1)
    Aug 22, 2010 14:31 (mike): created task 2: My second task (status: new, priority: high, project: 1)
    Aug 22, 2010 14:31 (mike): created task 3: My third task (status: open, priority: low, time: 4:00, project: 1)
    Aug 22, 2010 14:32 (mike): updated task 3: My third task (status: new)
    Aug 22, 2010 14:32 (mike): updated task 1: My very first task (date: Oct 10, 2010)
    Aug 22, 2010 14:32 (mike): created note 1: Sample note for task #1 (task 1)
    Aug 22, 2010 14:33 (mike): moved task 1: from project 1 to project 2
    Aug 22, 2010 14:33 (mike): deleted note 1: Sample note for task #1 (task 1)
    Aug 22, 2010 14:33 (mike): deleted task 1: My very first task with 1 note (project: 2)
    Aug 22, 2010 14:33 (mike): deleted project 2: My second project with 1 task

### License ###
Copyright (c) 2010 Michael Dvorkin

mike[at]dvorkin.net aka mike[at]fatfreecrm.com

THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Released under the Simplified BSD license. See LICENSE file for details.
