dynamics = {} -- All dynamic behaviours will be managed by Lua so that it knows which entity to issue which function

function dynamicBehaviour1(node)
    local position = { x = 0.0, y = 20.0, z = 0.0 }
    while true do
        print("[LUA]: Starting automation.")
        node:addTeleportBehaviour(position)
        coroutine.yield()
        position = { x = 50.0, y = 20.0, z = 0.0 }
        node:addMoveBehaviour(position, 5)
        coroutine.yield()
        position = { x = 100.0, y = 200.0, z = 0.0 }
        node:addMoveBehaviour(position, 5)
        coroutine.yield()
        local waypoint = { x = 100.0, y = 10000.0, z = 0.0 }
        position = { x = 1000.0, y = 2000.0, z = 0.0 }
        node:addCurveMoveBehaviour(waypoint, position, 10)
        coroutine.yield()
        position = { x = 0.0, y = 20.0, z = 0.0 }
    end
end

function IssueNextTask(node)
    -- check if coroutine is not dead
    if coroutine.status(dynamics[node].behaviour) ~= 'dead' then
        print("[LUA]: Issue next task.")
        -- resume method needs coroutine and any parameters
        coroutine.resume(dynamics[node].behaviour, node)
    end
end

function Init(node)
    dynamics[node] = {behaviour = coroutine.create(dynamicBehaviour1)}
    IssueNextTask(node)
end

-- Required ATM
function Update(deltaTime, node)
    --print("x: " .. GetMouseCoords().x)
    --print("y:" .. GetMouseCoords().y)
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
