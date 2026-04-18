import matplotlib.pyplot as plt

positions = []

with open("output.txt") as f:
    for line in f:
        nums = list(map(float, line.split()))
        step = []

        for i in range(0, len(nums)-1, 2):
            step.append((nums[i], nums[i+1]))

        positions.append(step)

plt.ion()

fig, ax = plt.subplots()

# initialize scatter
n = len(positions[0])
scat = ax.scatter([], [])

ax.set_xlim(-10, 15)
ax.set_ylim(160, 190)
ax.set_aspect('equal')

for frame in positions:
    x = [p[0] for p in frame]
    y = [p[1] for p in frame]

    scat.set_offsets(list(zip(x, y)))

    plt.pause(0.001)

plt.ioff()
plt.show()
