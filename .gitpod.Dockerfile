FROM gitpod/workspace-full:latest

RUN bash -c ". .nvm/nvm.sh \
    && nvm install 18.6.0 \
    && nvm use 18.6.0 \
    && nvm alias default 18.6.0"

RUN echo "nvm use default &>/dev/null" >> ~/.bashrc.d/51-nvm-fix