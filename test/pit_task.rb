# Copyright (c) 2010 Michael Dvorkin
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the Simplified BSD License (also
# known as the "2-Clause License" or "FreeBSD License".)
#
# This program is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.

module PitTest
  module Task

    def should_do_cascading_task_delete
      `#{@pit} project -c project`
      12.times do |t|
        `#{@pit} task -c task#{t}`
        `#{@pit} task`.should.match /\* #{t+1}:/m
        12.times do |n|
          `#{@pit} note -c task#{t}_note#{n}`
          `#{@pit} note`.should.match /\* #{t*12 + n+1}:/m
        end
      end
      `#{@pit} task`.should.match /\* 12:/m
      `#{@pit} note -e 21 hello21`
      `#{@pit} note -d 21`
      `#{@pit} note -e 22 hello22`
      `#{@pit} note -d 22`
      `#{@pit} task -d`
      `#{@pit} task`.should_not.match /[\s|\*] 12:/m

    end
  end
end
