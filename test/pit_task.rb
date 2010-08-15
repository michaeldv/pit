module Pit
  module Task

    def should_do_cascading_delete
      `#{@pit} project -c project`
      5.times do |t|
        `#{@pit} task -c task#{t}`
        5.times do |n|
          `#{@pit} note -c task#{t}_note#{n}`
        end
      end
      `#{@pit} task`.should.match /\* 5:/m
      `#{@pit} note -e 21 hello21`
      `#{@pit} note -d 21`
      `#{@pit} note -e 22 hello22`
      `#{@pit} note -d 22`
      `#{@pit} task -d`
      `#{@pit} task`.should_not.match /[\s|\*] 5:/m
    end
  end
end
