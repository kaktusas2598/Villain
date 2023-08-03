-- Required ATM
function Update(deltaTime, node)
end

-- Optional callback
function OnKey(key, node)
    --print("Key: " .. string.char(key))

    -- Simple move controller
    local position = node:getPosition()
    --104, 106, 107, 108 - h j k l
    if key == 104 then
        position.x = position.x + 1
    elseif key ==108 then
        position.x = position.x - 1
    elseif key ==106 then
        position.z = position.z + 1
    elseif key ==107 then
        position.z = position.z - 1
    end
    node:setPosition(position)
end
