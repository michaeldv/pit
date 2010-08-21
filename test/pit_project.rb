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
  module Project

    def should_create_project
      `#{@pit} project -c test`
      `#{@pit} project`.should.match /\* 1: \(.+\) \|active| test \(0 tasks\)/
    end

    def should_do_cascading_project_delete
      12.times do |p|
        `#{@pit} project -c project#{p}`
        `#{@pit} project`.should.match /\* #{p+1}:/m
        12.times do |t|
          `#{@pit} task -c project#{p}_task#{t}`
          `#{@pit} task`.should.match /\* #{p*12 + t+1}:/m
          12.times do |n|
            `#{@pit} note -c project#{p}_task#{t}_note#{n}`
            `#{@pit} note`.should.match /\* #{p*144 + t*12 + n+1}:/m
          end
        end
      end
      `#{@pit} project`.should.match /\* 12:/m
      `#{@pit} project -d`
      `#{@pit} project`.should_not.match /[\s|\*] 12:/m
      `#{@pit} project`.should.match /\s 11:/m
      `#{@pit} project -d 2`
      `#{@pit} project`.should_not.match /[\s|\*] 2:/m
      `#{@pit} project`.should.match /\s 1:/m
    end

  end
end
