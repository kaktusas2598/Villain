function Update(deltaTime, node)
    if (deltaTime > 0.017) then
        print("LOW FPS! Delta time: " .. deltaTime)
    end
    --print("Screen width: " .. getScreenWidth())
    --print("Node name: " .. node:getName())
end

-- Optional callback
function OnKey(key, node)
    if (node:getComponent("Camera")) then
        print("Camera compo found!")
    end

    if (key == 114) then -- 114 is code for 'r' in SDL2/Villain::KeyCode
        print("Setting node pos")
        local position = { x = 0.0, y = 20.0, z = 0.0 }
        node:setPosition(position)
    end
    local position = node:getPosition()
    print(position.x, position.y, position.z)
end
