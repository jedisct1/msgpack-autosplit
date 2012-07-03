Then /^the number of files for "(.*?)" should be (\d+)\.$/ do |glob, count|
  Dir[glob].count.should be(count.to_i)
end
