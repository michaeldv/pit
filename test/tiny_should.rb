# Copyright (c) 2010 Michael Dvorkin
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the Simplified BSD License (also
# known as the "2-Clause License" or "FreeBSD License".)
#
# This program is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.

# I am well aware there is Test::Unit, minitest, rspec, shoulda,
# bacon and a dozen of other testing frameworks.  I just had fun
# writing this one and frankly that's all I need for basic testing.
module TinyShould
  def should
    def self.equal(expected)
      raise Hand unless self == expected
      print '.'
    rescue => e
      e.oops "equal failed: #{self} != #{expected}"
    end

    def self.match(expected)
      raise Hand unless self =~ expected
      print '.'
    rescue => e
      e.oops "match failed: #{self} !~ /#{expected.source}/"
    end

    self
  end

  def should_not
    def self.equal(expected)
      raise Hand unless self != expected
      print '.'
    rescue => e
      e.oops "not equal failed: #{self} == #{expected}"
    end

    def self.match(expected)
      raise Hand unless self !~ expected
      print '.'
    rescue => e
      e.oops "not match failed: #{self} =~ /#{expected.source}/"
    end

    self
  end
end

class Hand < RuntimeError
  def oops(msg)
    puts "\n#{msg} in #{self.backtrace[1].sub(':', ' line ')}"
  end
end

class String
  include TinyShould
end

if $0 == __FILE__
  "123".should.equal "123"
  "123".should.equal "321"

  "123".should_not.equal "321"
  "123".should_not.equal "123"

  "abc".should.match /B/i
  "abc".should.match /xyz/

  "abc".should_not.match /xyz/
  "abc".should_not.match /abc/
end
