#!/usr/bin/env ruby
#
BASE = File.dirname(File.expand_path(__FILE__))
require "#{BASE}/pit_should"
require "#{BASE}/pit_date"
require "#{BASE}/pit_project"
require "#{BASE}/pit_task"
require "#{BASE}/pit_note"

class PitTestRunner
  def self.run
    runner = self.new
    STDOUT.sync = true
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
  end

  include Pit::Date
  include Pit::Project
  include Pit::Task
  include Pit::Note
end

PitTestRunner.run
