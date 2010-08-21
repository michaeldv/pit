#!/usr/bin/env ruby
#
# Copyright (c) 2010 Michael Dvorkin
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the Simplified BSD License (also
# known as the "2-Clause License" or "FreeBSD License".)
#
# This program is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.

BASE = File.dirname(File.expand_path(__FILE__))
require "#{BASE}/tiny_should"
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

  include PitTest::Date
  include PitTest::Project
  include PitTest::Task
  include PitTest::Note
end

PitTestRunner.run
