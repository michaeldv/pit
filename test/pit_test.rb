#!/usr/bin/env ruby
#
BASE = File.dirname(File.expand_path(__FILE__))
require "#{BASE}/pit_should"

class PitTestRunner
  def self.run
    runner = self.new
    runner.public_methods.grep(/^should_/).each do |method|
      runner.send(:before) if runner.respond_to?(:before)
      runner.send(method)
      runner.send(:after) if runner.respond_to?(:after)
    end
    puts
  end

  def initialize
    @pit = "#{BASE.sub(/test$/, 'bin')}/pit"
    if File.executable?(@pit)
      ENV['PITFILE'] = "#{BASE}/test.pitfile"
      puts "Testing #{@pit} with #{ENV['PITFILE']}"
    else
      raise("\nCould not run #{@pit}") 
    end
  end

  def before
    `#{@pit} init -f`
    `#{@pit} project -c test`
  end

  def should_parse_alpha_dates
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

PitTestRunner.run
