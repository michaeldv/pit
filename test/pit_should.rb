class Hand < RuntimeError
  def oops(msg)
    puts "\n#{msg} in #{self.backtrace[1].sub(':', ' line ')}"
  end
end

class String
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


