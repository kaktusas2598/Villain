function Update(deltaTime, node)
    if (deltaTime > 0.017) then
        print("LOW FPS! Delta time: " .. deltaTime)
    end
    --print("Screen width: " .. getScreenWidth())
    --print("Node name: " .. node:getName())
end

music = GetAudio("assets/audio/TownTheme.mp3")
Pitch = 1.0
-- Optional callback
function OnKey(key, node)
    --if (node:getComponent("Camera")) then
        --print("Camera compo found!")
    --end

    --music = GetAudio("themeMusic")
    music:setVolume(0.02)
    music:setPitch(Pitch)
    if (key == KeyCode("r")) then
        Pitch = Pitch - 0.01
        local position = { x = 0.0, y = 20.0, z = 0.0 }
        node:setPosition(position)
        music:play()
    end
    --local position = node:getPosition()
    --print(position.x, position.y, position.z)
end

function OnCollide(node, thisBody, otherBody)
    --print("Node: " .. node:getName())
    if not IsNil(otherBody) then
        --print("This mass: " .. thisBody:getMass() .. " Other mass: " .. otherBody:getMass())
        -- On touching other rigid body, give it some up force to lift it up
        local force = { x = 0.0, y = 10, z = 0.0 }
        ----otherBody:addLinearVelocity(force)
    end
end
