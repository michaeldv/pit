module Pit
  module Note
    def should_not_segfault
      `#{@pit} p -c test`
      `#{@pit} t -c test`
      10.times do |i|
        `#{@pit} n -c note#{i+1}`
      end
      5.times do |i|
        `#{@pit} n -d #{i+1}`
      end
      `#{@pit} n -c note11`
      `#{@pit} l`.should.match /created note 11/m
    end
  end
end
