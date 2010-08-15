module Pit
  module Project

    def should_create_project
      `#{@pit} project -c test`
      `#{@pit} project`.should.match /\* 1: \(.+\) \|active| test \(0 tasks\)/
    end

    def should_do_cascading_delete
      def should_do_cascading_delete
        3.times do |p|
          `#{@pit} project -c project#{p}`
          3.times do |t|
            `#{@pit} task -c project#{p}_task#{t}`
            3.times do |n|
              `#{@pit} note -c project#{p}_task#{t}_note#{n}`
            end
          end
        end
        `#{@pit} project`.should.match /\* 3:/m
        `#{@pit} project -d`
        `#{@pit} project`.should_not.match /[\s|\*] 3:/m
        `#{@pit} project`.should.match /\s 2:/m
        `#{@pit} project -d 2`
        `#{@pit} project`.should_not.match /[\s|\*] 2:/m
        `#{@pit} project`.should.match /\s 1:/m
      end
    end
  end
end
