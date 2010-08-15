module Pit
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
