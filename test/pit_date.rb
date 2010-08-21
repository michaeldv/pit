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
  module Date

    def should_parse_alpha_dates
      `#{@pit} project -c test`
      `#{@pit} task -c test -d "dec 1, 2011 19:30"`
      `#{@pit} task`.should.match /Dec 01, 2011 19:30/m
      `#{@pit} task -c test -d "dec 1, 2012 1:15pm"`
      `#{@pit} task`.should.match /Dec 01, 2012 13:15/m
      `#{@pit} task -c test -d "dec 1, 2013 11"`
      `#{@pit} task`.should.match /Dec 01, 2013 11:00/m
      `#{@pit} task -c test -d "dec 1, 2013 7pm"`
      `#{@pit} task`.should.match /Dec 01, 2013 19:00/m
      `#{@pit} task -c test -d "dec 1, 2013"`
      `#{@pit} task`.should.match /Dec 01, 2013      /m
      `#{@pit} task -c test -d "dec 1 5:55"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 05:55/m
      `#{@pit} task -c test -d "dec 1 2:40pm"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 14:40/m
      # `#{@pit} task -c test -d "dec 1 5"`
      # `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 05:00/m
      # `#{@pit} task -c test -d "dec 1 3pm"`
      # `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 15:00/m
      `#{@pit} task -c test -d "dec 1"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year}      /m
    end

    def should_parse_slash_dates
      `#{@pit} project -c test`
      `#{@pit} task -c test -d "12/1/2011 19:30"`
      `#{@pit} task`.should.match /Dec 01, 2011 19:30/m
      `#{@pit} task -c test -d "12/1/2012 1:15pm"`
      `#{@pit} task`.should.match /Dec 01, 2012 13:15/m
      `#{@pit} task -c test -d "12/1/2013 11"`
      `#{@pit} task`.should.match /Dec 01, 2013 11:00/m
      `#{@pit} task -c test -d "12/1/2013 7pm"`
      `#{@pit} task`.should.match /Dec 01, 2013 19:00/m
      `#{@pit} task -c test -d "12/1/2013"`
      `#{@pit} task`.should.match /Dec 01, 2013      /m
      `#{@pit} task -c test -d "12/1 5:55"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 05:55/m
      `#{@pit} task -c test -d "12/1 2:40pm"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 14:40/m
      # `#{@pit} task -c test -d "12/1 5"`
      # `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 05:00/m
      # `#{@pit} task -c test -d "12/1 3pm"`
      # `#{@pit} task`.should.match /Dec 01, #{Time.now.year} 15:00/m
      `#{@pit} task -c test -d "12/1"`
      `#{@pit} task`.should.match /Dec 01, #{Time.now.year}      /m
    end
  end
end

    