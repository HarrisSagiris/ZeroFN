FROM node:22-alpine AS base

ENV PNPM_HOME="/pnpm"
ENV PATH="$PNPM_HOME:$PATH"
RUN wget -qO- https://get.pnpm.io/install.sh | ENV="$HOME/.shrc" SHELL="$(which sh)" sh -

# builder
FROM base AS builder
WORKDIR /app

# install dependencies
COPY package.json ./
RUN pnpm install

# Copy application files
COPY . .

ENV NODE_ENV="production"
RUN pnpm run build

# production
FROM base AS production
WORKDIR /app

COPY --from=builder /app/build build
COPY package.json ./
RUN pnpm install --prod

EXPOSE 3000

CMD [ "node", "build/index.js" ]